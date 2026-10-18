// W4-05 dashcam footage source contract test.
//
// Always-on (no ROM / footage / ffmpeg needed): naming-table consistency, race
// table -> route bounds, clip-name grammar, fit constants (and that no legacy
// mapping crept back), directory resolution incl. the never-release-id rule,
// directory scan / import, and graceful absence of the ffmpeg runtime.
// Conditional (prints SKIP and still exits 0 when absent):
//   ROM   build/rom/SLUS_203.98 or $RT_ELF   : race-table bytes, no-movie-strings,
//                                              no IPU access (with VIF1/GIF control)
//   disc  $RT_DISC or ~/Library/.../disc     : COURSE/<stem>.BIN exists per route
//   ffmpeg/ffprobe on PATH                   : synthetic 1920x1080 clip decode
//   $RT_DASHCAM_DIR with real clips          : probe every real clip
#include "media/dashcam_source.h"

#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <set>
#include <string>
#include <vector>

namespace fs = std::filesystem;
using namespace rt::native::media;

namespace {

int g_failures = 0;
int g_checks = 0;
int g_skips = 0;

void check(bool ok, const std::string& what) {
    ++g_checks;
    if (!ok) {
        ++g_failures;
        std::cerr << "FAIL: " << what << "\n";
    }
}

void skip(const std::string& what) {
    ++g_skips;
    std::cout << "SKIP: " << what << "\n";
}

bool near(float a, float b, float eps = 1e-4f) { return std::fabs(a - b) <= eps; }

std::uint32_t rd32(const std::vector<std::uint8_t>& d, std::size_t off) {
    return std::uint32_t(d[off]) | std::uint32_t(d[off + 1]) << 8 | std::uint32_t(d[off + 2]) << 16 |
           std::uint32_t(d[off + 3]) << 24;
}
std::uint16_t rd16(const std::vector<std::uint8_t>& d, std::size_t off) {
    return std::uint16_t(d[off] | d[off + 1] << 8);
}

struct Seg {
    std::uint32_t off, vaddr, filesz, flags;
};

struct Elf {
    std::vector<std::uint8_t> data;
    std::vector<Seg> segs;
    const std::uint8_t* at(std::uint32_t vaddr, std::uint32_t n) const {
        for (const auto& s : segs)
            if (vaddr >= s.vaddr && vaddr + n <= s.vaddr + s.filesz) return data.data() + s.off + (vaddr - s.vaddr);
        return nullptr;
    }
};

bool loadElf(const fs::path& p, Elf& elf) {
    std::ifstream in(p, std::ios::binary);
    if (!in) return false;
    elf.data.assign(std::istreambuf_iterator<char>(in), {});
    if (elf.data.size() < 0x34 || std::memcmp(elf.data.data(), "\x7f" "ELF", 4) != 0) return false;
    const std::uint32_t phoff = rd32(elf.data, 0x1C);
    const std::uint16_t phentsize = rd16(elf.data, 0x2A), phnum = rd16(elf.data, 0x2C);
    for (std::uint16_t i = 0; i < phnum; ++i) {
        const std::size_t b = phoff + std::size_t(i) * phentsize;
        if (b + 32 > elf.data.size()) return false;
        if (rd32(elf.data, b) != 1) continue; // PT_LOAD
        Seg s{rd32(elf.data, b + 4), rd32(elf.data, b + 8), rd32(elf.data, b + 16), rd32(elf.data, b + 24)};
        if (std::size_t(s.off) + s.filesz > elf.data.size()) return false;
        elf.segs.push_back(s);
    }
    return !elf.segs.empty();
}

// Count lui-based accesses (<=12 insns, same base reg) into [lo,hi).
int countMmioAccess(const Elf& elf, std::uint32_t lo, std::uint32_t hi) {
    int hits = 0;
    for (const auto& s : elf.segs) {
        if (!(s.flags & 1)) continue; // executable segment only
        const std::uint32_t n = s.filesz / 4;
        auto word = [&](std::uint32_t i) { return rd32(elf.data, s.off + std::size_t(i) * 4); };
        for (std::uint32_t i = 0; i < n; ++i) {
            const std::uint32_t w = word(i);
            if ((w >> 26) != 0x0F) continue;
            const std::uint32_t rt = (w >> 16) & 31, upper = (w & 0xFFFF) << 16;
            for (std::uint32_t j = i + 1; j < n && j <= i + 12; ++j) {
                const std::uint32_t w2 = word(j), op = w2 >> 26, rs = (w2 >> 21) & 31;
                static const std::set<std::uint32_t> itype{0x08, 0x09, 0x0D, 0x1E, 0x1F, 0x20, 0x21, 0x23,
                                                           0x24, 0x25, 0x28, 0x29, 0x2B, 0x37, 0x3F};
                if (itype.count(op) && rs == rt) {
                    const std::uint32_t imm = w2 & 0xFFFF;
                    const std::uint32_t ea =
                        op == 0x0D ? (upper | imm) : upper + std::uint32_t(std::int32_t(std::int16_t(imm)));
                    if (ea >= lo && ea < hi) ++hits;
                }
                if (op == 0x0F && ((w2 >> 16) & 31) == rt) break;
            }
        }
    }
    return hits;
}

std::string upper(std::string s) {
    for (char& c : s) c = char(std::toupper(static_cast<unsigned char>(c)));
    return s;
}

void touch(const fs::path& p, const std::string& content = "x") {
    std::ofstream(p, std::ios::binary) << content;
}

} // namespace

int main() {
    // ------------------------------------------------------------ naming table
    std::set<std::string> stems;
    for (std::size_t i = 0; i < kDashcamRoutes.size(); ++i) {
        const auto& r = kDashcamRoutes[i];
        check(r.id == i, "route ids are dense 0..17 in order");
        const std::string stem = r.packStem;
        check(stem.size() == 3 && stem[0] == 'C' && std::isdigit((unsigned char)stem[1]) &&
                  std::isdigit((unsigned char)stem[2]),
              "stem has COURSE pack shape Cnn: " + stem);
        stems.insert(stem);
        check(packStemFor(r.id) == stem, "packStemFor round trip " + stem);
        check(routeForStem(stem) == r.id, "routeForStem round trip " + stem);
        check(routeForStem(std::string(1, 'c') + stem.substr(1)) == r.id, "routeForStem is case-insensitive");
    }
    const std::set<std::string> expected{"C00", "C01", "C02", "C03", "C04", "C05", "C06", "C07", "C08",
                                         "C09", "C10", "C11", "C12", "C13", "C14", "C18", "C19", "C20"};
    check(stems == expected, "stem set = C00..C14,C18,C19,C20 (nav-index-binding.md rows 0..17)");
    check(stems.count("C15") == 0 && stems.count("C16") == 0 && stems.count("C17") == 0,
          "no C15/C17 (absent on disc) and no C16 (orphan)");
    check(packStemFor(kCourseRouteCount).empty() && !isCourseRoute(kCourseRouteCount),
          "id 18 (ACTION/A00) is not a footage route");
    check(!routeForStem("A00") && !routeForStem("C16") && !routeForStem(""), "non-course stems rejected");

    for (auto v : kAdventureRaceRoute) check(isCourseRoute(v), "adventure race routes are course packs");
    for (auto v : kQuickRaceRoute) check(isCourseRoute(v), "quick race routes are course packs");
    check(routeForAdventureRace(0) == RouteId(0) && routeForAdventureRace(24) == RouteId(17) &&
              !routeForAdventureRace(35) && !routeForAdventureRace(-1),
          "adventure race index bounds (0x23 records)");
    check(routeForQuickRace(7) == RouteId(17) && !routeForQuickRace(8), "quick race index bounds (8 records)");

    // ------------------------------------------------------------ clip grammar
    check(parseClipName("C00.mp4") && parseClipName("C00.mp4")->route == 0 && parseClipName("C00.mp4")->take == 0,
          "C00.mp4 -> route 0 take 0");
    check(parseClipName("c20_12.MOV") && parseClipName("c20_12.MOV")->route == 17 &&
              parseClipName("c20_12.MOV")->take == 12,
          "c20_12.MOV -> route 17 take 12");
    check(parseClipName("C18.webm") && parseClipName("C18.webm")->route == 15, "C18 -> route 15");
    for (const char* bad : {"C00.txt", "C00_.mp4", "C00_0.mp4", "C00_100.mp4", "C00_x.mp4", "C15.mp4", "C16.mp4",
                            "A00.mp4", ".mp4", "C00", "C00_1_2.mp4", "drive_top.mp4"})
        check(!parseClipName(bad), std::string("rejected clip name ") + bad);
    check(canonicalClipName(0) == "C00.mp4" && canonicalClipName(15, 2, ".MOV") == "C18_2.mov" &&
              canonicalClipName(18).empty(),
          "canonical clip names");
    for (const auto& r : kDashcamRoutes) {
        const auto p = parseClipName(canonicalClipName(r.id, 3));
        check(p && p->route == r.id && p->take == 3, "canonical name parses back");
    }

    // ------------------------------------------------------------ fit constants
    check(kDashcamSrcW == 1920 && kDashcamSrcH == 1080, "source is 1920x1080");
    check(near(kDashcamScale, 1.5f) && near(kDashcamXOff, 0.f) && near(kDashcamYOff, 317.f),
          "fit = scale 1.5, xoff 0, yoff 317");
    check(near(kDashcamFitIoU, 0.733f), "fit IoU recorded 73.3%");
    check(!near(kDashcamScale, 2.25f) && !near(kDashcamYOff, 54.f), "legacy 2.25 + 54px mapping not used");
    check(!near(kDashcamScale, 2.4107f, 1e-3f) && !near(kDashcamXOff, 188.f), "legacy 2.4107 + xoff 188 not used");
    check(near(dashcamSrcX(100.f), 150.f) && near(dashcamSrcY(100.f), 467.f), "panel -> source law");
    check(near(dashcamPanelX(dashcamSrcX(37.25f)), 37.25f) && near(dashcamPanelY(dashcamSrcY(-12.5f)), -12.5f),
          "source -> panel inverse");
    {
        const SrcRect c = dashcamSourceCrop(640.f, 448.f);
        check(near(c.x, 0.f) && near(c.y, 317.f) && near(c.w, 960.f) && near(c.h, 672.f),
              "640x448 panel samples (0,317,960,672)");
        const SrcRect big = dashcamSourceCrop(2000.f, 2000.f);
        check(near(big.x + big.w, 1920.f) && near(big.y + big.h, 1080.f), "crop clipped to source frame");
    }

    // ------------------------------------------------------------ directory resolution
    {
        DashcamEnv e;
        e.dashcamDir = "/x/override";
        e.saveDir = "/x/save";
        check(resolveDashcamDir(e) == fs::path("/x/override"), "RT_DASHCAM_DIR wins");
        e.dashcamDir.reset();
        check(resolveDashcamDir(e) == fs::path("/x/save/dashcam"), "RT_SAVE_DIR/dashcam on desktop");
        e.saveDir.reset();
        e.dataDir = "/x/data";
        check(resolveDashcamDir(e) == fs::path("/x/data/dashcam"), "app-local RT_DATA_DIR/dashcam");
        DashcamEnv bare;
        check(resolveDashcamDir(bare) == fs::path("dashcam"), "last resort ./dashcam");

        DashcamEnv a;
        a.platform = HostPlatform::Android;
        check(resolveDashcamDir(a) == fs::path("/data/data/io.github.silentsudin.roadtrip.dev/files/dashcam"),
              "android default = dev package files dir");
        a.androidFilesDir = "/data/user/0/io.github.silentsudin.roadtrip.dev/files";
        check(resolveDashcamDir(a) == fs::path("/data/user/0/io.github.silentsudin.roadtrip.dev/files/dashcam"),
              "android uses supplied dev files dir");
        a.androidFilesDir = "/data/user/0/io.github.silentsudin.roadtrip/files";
        check(resolveDashcamDir(a) == fs::path("/data/data/io.github.silentsudin.roadtrip.dev/files/dashcam"),
              "android release files dir is refused");
        a.dashcamDir = "/data/data/io.github.silentsudin.roadtrip/files/dashcam";
        check(!pathUsesReleasePackage(resolveDashcamDir(a)), "release override refused");
        check(pathUsesReleasePackage("/data/data/io.github.silentsudin.roadtrip/files") &&
                  !pathUsesReleasePackage("/data/data/io.github.silentsudin.roadtrip.dev/files"),
              "release-id detection is component exact");
        check(kAndroidDevPackage != kAndroidReleasePackage &&
                  std::string(kAndroidDevFilesDir).find(std::string(kAndroidDevPackage)) != std::string::npos,
              "dev files dir names the dev package");
    }

    // ------------------------------------------------------------ library scan/import
    const fs::path tmp = fs::temp_directory_path() / "rt-w5-dashcam-source-test";
    std::error_code ec;
    fs::remove_all(tmp, ec);
    {
        DashcamLibrary missing(tmp / "nope");
        check(missing.clipCount() == 0 && missing.availability(0) == Availability::Missing &&
                  missing.pathFor(0).empty(),
              "missing directory = no clips, not an error");
        check(missing.availability(18) == Availability::NotACourse, "id 18 NotACourse");

        fs::create_directories(tmp / "lib" / "C02.mp4", ec); // a directory named like a clip
        for (const char* n : {"C00.mov", "C00.mp4", "c00_2.mkv", "C00_1.webm", "C05.m4v", "C01.txt", "readme.md",
                              "C16.mp4", "A00.mp4"})
            touch(tmp / "lib" / n);
        DashcamLibrary lib(tmp / "lib");
        check(lib.clipCount() == 5, "5 valid clips indexed (got " + std::to_string(lib.clipCount()) + ")");
        const auto c0 = lib.clipsFor(0);
        check(c0.size() == 4, "route 0 has 4 clips");
        if (c0.size() == 4) {
            check(c0[0].path.filename() == "C00.mp4" && c0[1].path.filename() == "C00.mov" &&
                      c0[2].take == 1 && c0[3].take == 2,
                  "ordering: take, then extension priority (.mp4 before .mov)");
        }
        check(lib.pathFor(0).filename() == "C00.mp4", "pathFor = primary");
        check(lib.availability(5) == Availability::Present && lib.availability(1) == Availability::Missing &&
                  lib.availability(2) == Availability::Missing,
              "availability Present/Missing (directory named like a clip ignored)");

        std::string err;
        const auto bad = lib.importClip(1, tmp / "lib" / "C01.txt", 0, false, &err);
        check(!bad && !err.empty(), "import rejects non-video extension");
        const auto imported = lib.importClip(1, tmp / "lib" / "C05.m4v", 0, false, &err);
        check(imported && imported->filename() == "C01.m4v" && lib.availability(1) == Availability::Present,
              "import canonicalises name and rescans");
        check(!lib.importClip(1, tmp / "lib" / "C05.m4v", 0, false, &err), "import refuses overwrite by default");
        check(lib.importClip(1, tmp / "lib" / "C05.m4v", 0, true, &err).has_value(), "import overwrite opt-in");
        check(!lib.importClip(18, tmp / "lib" / "C05.m4v", 0, false, &err), "import refuses non-course route");
        DashcamLibrary release("/data/data/io.github.silentsudin.roadtrip/files/dashcam");
        check(!release.importClip(0, tmp / "lib" / "C05.m4v", 0, false, &err), "import refuses release dir");
    }

    // ------------------------------------------------------------ graceful absence
    {
        FfmpegFrameProvider none(ToolPaths{});
        check(!none.isRuntimeAvailable(), "no tools -> runtime unavailable");
        std::string err;
        check(!none.probe(tmp / "lib" / "C00.mp4", &err) && !err.empty(), "probe fails cleanly without ffprobe");
        check(!none.frameAt(tmp / "lib" / "C00.mp4", 0.0, 8, 8).valid(), "frameAt invalid without ffmpeg");
        check(!none.panelFrameAt(tmp / "lib" / "C00.mp4", 0.0, 8, 8).valid(), "panelFrameAt invalid without ffmpeg");
        const ToolPaths emptyPath = locateTools(std::string(""), std::nullopt, std::nullopt);
        check(emptyPath.ffmpeg.empty() && emptyPath.ffprobe.empty(), "empty PATH finds no tools");
        const ToolPaths bogus = locateTools(std::string("/nonexistent-rt-w5"), std::string("/nonexistent/ffmpeg"),
                                            std::nullopt);
        check(bogus.ffmpeg.empty(), "non-executable override ignored");

        check(decidePresentation(Availability::Present, false) == Presentation::PlaceholderStill,
              "present + no runtime -> placeholder");
        check(decidePresentation(Availability::Present, true) == Presentation::Footage, "present + runtime -> footage");
        check(decidePresentation(Availability::Missing, true) == Presentation::Skip, "missing -> skip");
        check(decidePresentation(Availability::NotACourse, true) == Presentation::Skip, "not a course -> skip");

        DashcamLibrary lib(tmp / "lib");
        Presentation p{};
        const RgbaFrame ph = none.presentRoute(lib, 0, 1.0, 16, 9, &p);
        check(p == Presentation::PlaceholderStill && ph.valid() && ph.pixels[0] == kPlaceholderRgba[0] &&
                  ph.pixels[3] == 255,
              "presentRoute placeholder still when runtime absent");
        const RgbaFrame sk = none.presentRoute(lib, 3, 1.0, 16, 9, &p);
        check(p == Presentation::Skip && !sk.valid(), "presentRoute skip when clip missing");

        // A runtime that exists but cannot decode the (fake) clip degrades too.
        FfmpegFrameProvider real;
        if (real.isRuntimeAvailable()) {
            const RgbaFrame f = real.presentRoute(lib, 0, 0.0, 16, 9, &p);
            check(p == Presentation::PlaceholderStill && f.valid(), "undecodable clip -> placeholder");
        }
    }

    check(shellQuote("a'b c") == "'a'\\''b c'", "POSIX shell quoting");
    {
        const auto info = parseProbeOutput("width=1920\nheight=1080\nr_frame_rate=30000/1001\navg_frame_rate=0/0\n"
                                           "nb_frames=N/A\nduration=153.784178\n");
        check(info && info->width == 1920 && info->height == 1080 && std::fabs(info->fps - 29.97) < 0.01 &&
                  info->frameCount == 0 && std::fabs(info->durationSec - 153.784178) < 1e-6,
              "ffprobe output parse");
        check(!parseProbeOutput("duration=1.0\n"), "probe without a video stream rejected");
    }

    // ------------------------------------------------------------ ROM grounding
    {
        const char* envElf = std::getenv("RT_ELF");
        const fs::path elfPath = envElf ? fs::path(envElf) : fs::path("build/rom/SLUS_203.98");
        Elf elf;
        if (!fs::exists(elfPath) || !loadElf(elfPath, elf)) {
            skip("ROM grounding (no ELF at " + elfPath.string() + "; NO_ELF rule)");
        } else {
            int match = 0;
            for (std::size_t i = 0; i < kAdventureRaceRoute.size(); ++i) {
                const auto* b = elf.at(kAdventureRaceTableVaddr + std::uint32_t(i) * kRaceRecordStride, 16);
                match += (b && b[0] == kAdventureRaceRoute[i]) ? 1 : 0;
            }
            check(match == int(kAdventureRaceRoute.size()), "adventure race table bytes match ELF (" +
                                                                std::to_string(match) + "/35)");
            int qmatch = 0;
            for (std::size_t i = 0; i < kQuickRaceRoute.size(); ++i) {
                const auto* b = elf.at(kQuickRaceTableVaddr + std::uint32_t(i) * kRaceRecordStride, 16);
                qmatch += (b && b[0] == kQuickRaceRoute[i]) ? 1 : 0;
            }
            check(qmatch == 8, "quick race table bytes match ELF (" + std::to_string(qmatch) + "/8)");

            // No movie container names anywhere in the loaded image; the IRX
            // path strings prove the scan sees cdrom0 names at all.
            int movieNames = 0, cdromNames = 0;
            for (const auto& s : elf.segs) {
                std::string run;
                for (std::uint32_t k = 0; k <= s.filesz; ++k) {
                    const char c = k < s.filesz ? char(elf.data[s.off + k]) : '\0';
                    if (c >= 0x20 && c < 0x7f) {
                        run += c;
                        continue;
                    }
                    if (run.size() >= 4) {
                        const std::string u = upper(run);
                        if (u.find("CDROM0:") != std::string::npos) ++cdromNames;
                        if (u.find("SCEMPEG") != std::string::npos) ++movieNames;
                        // File-name shape only: <alnum><ext> then end-of-string or ";1".
                        for (const std::string ext : {".PSS", ".IPU", ".M2V", ".AVI", ".MPG", ".MPEG", ".MP4",
                                                      ".MOV", ".AVP", ".STR", ".VOB"}) {
                            for (std::size_t at = u.find(ext); at != std::string::npos; at = u.find(ext, at + 1)) {
                                const std::size_t end = at + ext.size();
                                const bool tail = end == u.size() || u[end] == ';';
                                const bool head = at > 0 && std::isalnum(static_cast<unsigned char>(u[at - 1]));
                                movieNames += (tail && head) ? 1 : 0;
                            }
                        }
                    }
                    run.clear();
                }
            }
            check(cdromNames == 8, "8 cdrom0: names (IRX + IOPRP) found (got " + std::to_string(cdromNames) + ")");
            check(movieNames == 0, "no movie container names in ELF (got " + std::to_string(movieNames) + ")");

            const int ipu = countMmioAccess(elf, 0x10002000, 0x10002040);
            const int ipuFrom = countMmioAccess(elf, 0x1000B000, 0x1000B100);
            const int ipuTo = countMmioAccess(elf, 0x1000B400, 0x1000B500);
            const int vif1Dma = countMmioAccess(elf, 0x10009000, 0x10009100);
            const int gifDma = countMmioAccess(elf, 0x1000A000, 0x1000A100);
            check(vif1Dma > 0 && gifDma > 0, "MMIO scanner positive control (VIF1/GIF DMA) fires");
            check(ipu == 0 && ipuFrom == 0 && ipuTo == 0, "no IPU / IPU-DMA access in .text");
            std::cout << "ROM: race_tables=" << match << "/35+" << qmatch << "/8 movie_names=" << movieNames
                      << " cdrom_names=" << cdromNames << " ipu=" << ipu << "/" << ipuFrom << "/" << ipuTo
                      << " control_vif1=" << vif1Dma << " control_gif=" << gifDma << "\n";
        }
    }

    // ------------------------------------------------------------ disc grounding
    {
        fs::path disc;
        if (const char* d = std::getenv("RT_DISC")) disc = d;
        else if (const char* h = std::getenv("HOME")) disc = fs::path(h) / "Library/Application Support/RoadTripRecomp/disc";
        if (disc.empty() || !fs::is_directory(disc / "COURSE", ec)) {
            skip("disc grounding (no COURSE/ under " + disc.string() + ")");
        } else {
            int present = 0;
            for (const auto& r : kDashcamRoutes)
                present += fs::is_regular_file(disc / "COURSE" / (std::string(r.packStem) + ".BIN"), ec) ? 1 : 0;
            check(present == 18, "every route stem names a disc COURSE pack (" + std::to_string(present) + "/18)");
            std::cout << "DISC: course_packs=" << present << "/18\n";
        }
    }

    // ------------------------------------------------------------ live ffmpeg path
    {
        FfmpegFrameProvider provider;
        if (!provider.isRuntimeAvailable()) {
            skip("ffmpeg/ffprobe not on PATH; footage decode path untested (placeholder policy verified above)");
        } else {
            const fs::path dir = tmp / "live";
            fs::create_directories(dir, ec);
            const fs::path src = dir / "src.mp4";
            const std::string gen = shellQuote(provider.tools().ffmpeg.string()) +
                                    " -v error -nostdin -y -f lavfi -i testsrc2=size=1920x1080:rate=30:duration=1"
                                    " -pix_fmt yuv420p " +
                                    shellQuote(src.string());
            if (std::system(gen.c_str()) != 0 || !fs::exists(src)) {
                skip("ffmpeg present but lavfi testsrc2 generation failed");
            } else {
                DashcamLibrary lib(dir);
                std::string err;
                const auto dest = lib.importClip(0, src, 0, false, &err);
                check(dest.has_value(), "import synthetic clip: " + err);
                const auto info = provider.probe(lib.pathFor(0), &err);
                check(info && info->width == 1920 && info->height == 1080 && std::fabs(info->fps - 30.0) < 0.01,
                      "probe synthetic 1920x1080@30: " + err);
                const RgbaFrame full = provider.frameAt(lib.pathFor(0), 0.5, 64, 36, &err);
                check(full.valid() && full.width == 64 && full.height == 36, "frameAt 64x36: " + err);
                const RgbaFrame panel = provider.panelFrameAt(lib.pathFor(0), 0.5, 160, 112, &err);
                check(panel.valid(), "panelFrameAt 160x112: " + err);
                Presentation p{};
                const RgbaFrame shown = provider.presentRoute(lib, 0, 0.5, 160, 112, &p);
                check(p == Presentation::Footage && shown.valid(), "presentRoute footage with runtime");
                bool differs = false;
                for (std::size_t i = 0; i + 3 < shown.pixels.size() && !differs; i += 4)
                    differs = shown.pixels[i] != kPlaceholderRgba[0] || shown.pixels[i + 1] != kPlaceholderRgba[1];
                check(differs, "decoded footage is not the placeholder");
                std::cout << "FFMPEG: probe=" << (info ? info->width : 0) << "x" << (info ? info->height : 0)
                          << " fps=" << (info ? info->fps : 0) << " panel=" << panel.width << "x" << panel.height
                          << "\n";
            }
        }
    }

    // ------------------------------------------------------------ real footage (dev)
    {
        const char* real = std::getenv("RT_DASHCAM_DIR");
        FfmpegFrameProvider provider;
        if (!real) {
            skip("RT_DASHCAM_DIR unset; no real dashcam footage probed");
        } else {
            DashcamLibrary lib{fs::path(real)};
            if (lib.clipCount() == 0 || !provider.isRuntimeAvailable()) {
                skip("RT_DASHCAM_DIR has no clips or ffprobe missing");
            } else {
                int ok = 0;
                for (const auto& r : kDashcamRoutes)
                    for (const auto& c : lib.clipsFor(r.id)) ok += provider.probe(c.path).has_value() ? 1 : 0;
                check(ok == int(lib.clipCount()), "every real clip probes");
                std::cout << "REAL: clips=" << lib.clipCount() << " probed=" << ok << "\n";
            }
        }
    }

    fs::remove_all(tmp, ec);
    std::cout << "SCORE dashcam_source checks=" << g_checks << " failures=" << g_failures << " skips=" << g_skips
              << (g_failures ? " FAIL" : " PASS") << "\n";
    return g_failures ? 1 : 0;
}
