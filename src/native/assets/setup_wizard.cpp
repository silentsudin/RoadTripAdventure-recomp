#include "setup_wizard.h"

#include "iso_extract.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <thread>

#ifdef __ANDROID__
#include <android/log.h>
#endif

namespace rt::native::assets {
namespace {
constexpr std::uint32_t kBootElfCrc = 0x5A49851Du;

void log(const std::string& message) {
    std::cout << message << '\n';
#ifdef __ANDROID__
    __android_log_write(ANDROID_LOG_INFO, "RoadTrip", message.c_str());
#endif
}

namespace {
// CRC32 (reflected, 0xEDB88320) slicing-by-1 table; the previous per-bit
// loop ran ~40 MB/s and the boot-time manifest validation re-CRCed every
// extracted file, costing ~6 s on desktop and ~60 s+ on Thor (measured via
// sample: isSetupManifestValid/crcFile dominated startup, delaying the
// first present and the test-socket bind).
struct Crc32Table {
    std::uint32_t v[256];
    Crc32Table() {
        for (std::uint32_t i = 0; i < 256; ++i) {
            std::uint32_t c = i;
            for (int bit = 0; bit != 8; ++bit) c = (c >> 1) ^ (0xEDB88320u & -(c & 1u));
            v[i] = c;
        }
    }
};
const Crc32Table kCrc32;
} // namespace

std::uint32_t crcFile(const std::filesystem::path& path) {
    if (std::ifstream f{path, std::ios::binary}; f) {
        std::uint32_t crc = 0xFFFFFFFFu;
        // Heap, never stack: SDL's Android thread stacks are small and a 1 MiB
        // stack buffer overflowed crcFile at boot (Thor crash: "stack pointer
        // is in a non-existent map"). Desktop survived only on its 8 MB stack.
        thread_local std::vector<unsigned char> buffer(1u << 20);
        while (f.read(reinterpret_cast<char*>(buffer.data()), buffer.size()) || f.gcount() != 0)
            for (std::size_t n = static_cast<std::size_t>(f.gcount()), i = 0; i < n; ++i)
                crc = (crc >> 8) ^ kCrc32.v[(crc ^ buffer[i]) & 0xFFu];
        return crc ^ 0xFFFFFFFFu;
    }
    return 0xFFFFFFFFu;
}

std::filesystem::path dataRoot() {
    if (const char* value = std::getenv("RT_DATA_DIR")) return value;
    if (const char* home = std::getenv("HOME")) return std::filesystem::path(home) / "Library/Application Support/RoadTripRecomp";
    return {};
}

std::filesystem::path findDisc() {
    if (const char* value = std::getenv("RT_DISC"); value && *value && std::filesystem::is_directory(value)) return value;
    const auto data = dataRoot();
    for (const auto& candidate : {data / "rom", data / "disc", std::filesystem::path("/storage/emulated/0/Download/RoadTripDisc")})
        if (std::filesystem::is_directory(candidate)) return candidate;
    std::error_code ec;
    const auto downloads = std::filesystem::path("/storage/emulated/0/Download");
    if (std::filesystem::is_directory(downloads, ec)) for (const auto& entry : std::filesystem::directory_iterator(downloads, ec))
        if (entry.path().extension() == ".iso") log("rom: ISO candidate skipped-not-yet " + entry.path().filename().string());
    return {};
}

bool parseManifest(const std::filesystem::path& manifest, SetupManifest& result) {
    std::ifstream in(manifest);
    std::string line;
    while (std::getline(in, line)) {
        if (line.rfind("file\t", 0) != 0) continue;
        std::istringstream fields(line.substr(5));
        SetupManifestFile file;
        if (!std::getline(fields, file.path, '\t') || !(fields >> file.size) || fields.get() != '\t' || !(fields >> std::hex >> file.crc32)) return false;
        result.files.push_back(std::move(file));
    }
    return !result.files.empty();
}

void collect(const std::filesystem::path& root, const char* relative, const char* bucket,
             std::vector<std::pair<std::filesystem::path, std::filesystem::path>>& out) {
    const auto source = root / relative;
    if (!std::filesystem::exists(source)) return;
    if (std::filesystem::is_regular_file(source)) { out.emplace_back(source, std::filesystem::path(bucket) / source.filename()); return; }
    for (const auto& entry : std::filesystem::recursive_directory_iterator(source))
        if (entry.is_regular_file()) out.emplace_back(entry.path(), std::filesystem::path(bucket) / std::filesystem::relative(entry.path(), source));
}
} // namespace

std::uint32_t crc32(const std::vector<unsigned char>& bytes) {
    std::uint32_t crc = 0xFFFFFFFFu;
    for (unsigned char byte : bytes) { crc ^= byte; for (int bit = 0; bit != 8; ++bit) crc = (crc >> 1) ^ (0xEDB88320u & -(crc & 1u)); }
    return crc ^ 0xFFFFFFFFu;
}

bool hasExpectedBootElfCrc(const std::filesystem::path& elf) { return std::filesystem::is_regular_file(elf) && crcFile(elf) == kBootElfCrc; }

bool writeSetupManifest(const std::filesystem::path& path, const SetupManifest& manifest) {
    std::error_code ec; std::filesystem::create_directories(path.parent_path(), ec);
    std::ofstream out(path);
    if (!out) return false;
    out << "{\n  \"format\": \"roadtrip-native-assets-v1\",\n";
    for (const auto& file : manifest.files) out << "file\t" << file.path << '\t' << file.size << '\t' << std::hex << file.crc32 << std::dec << '\n';
    return static_cast<bool>(out);
}

namespace {
// Manifest file-set check.  Fast mode (startup default) validates existence
// and size for every entry; full mode additionally re-CRCs.  Full validation
// always runs once immediately after extraction and on demand via
// RT_APP_SETUP_VERIFY=full.  The previous launch-time full re-CRC cost
// ~6 s desktop / ~60 s+ on Thor and pushed the test-socket bind past the
// rehearsal handshake window.
bool checkManifestFiles(const std::filesystem::path& assetsRoot, bool fullCrc) {
    SetupManifest manifest;
    if (!parseManifest(assetsRoot / "manifest.json", manifest)) return false;
    for (const auto& file : manifest.files) {
        const auto path = assetsRoot / file.path;
        std::error_code ec;
        if (!std::filesystem::is_regular_file(path, ec) ||
            std::filesystem::file_size(path, ec) != file.size) return false;
        if (fullCrc && crcFile(path) != file.crc32) return false;
    }
    return true;
}
} // namespace

bool isSetupManifestValid(const std::filesystem::path& assetsRoot) {
    // Public contract: full CRC validation of every manifest entry.
    return checkManifestFiles(assetsRoot, true);
}

SetupScreenText describeSetupScreen(const SetupScreenView& v) {
    SetupScreenText t;
    switch (v.kind) {
    case SetupScreenView::Kind::Failed:
        t.heading = "Setup didn't finish";
        t.body = v.error + (v.logTail.empty() ? " Try again." : " Try again; if it keeps failing, the details below are "
                                                                "saved in the app's build log.");
        t.buttons = {"Close", "Try again"};
        t.defaultFocus = 1;
        break;
    case SetupScreenView::Kind::Welcome:
        t.heading = "Welcome!";
        t.body = "Road Trip needs your own copy of the game. Choose the disc image you made from it "
                 "(.bin, .iso or .chd).";
        t.footer = v.note;
        t.buttons = {"Choose disc image"};
        break;
    case SetupScreenView::Kind::Warning:
        t.heading = "This disc image looks different";
        t.body = "It isn't the usual copy of Road Trip (USA), so it may be damaged or from another release. "
                 "It might still work.";
        t.buttons = {"Use it anyway", "Choose another"};
        t.defaultFocus = 1;
        if (v.warning == SetupWarning::Damaged) {
            t.heading = "This disc image is damaged";
            t.body = "Part of this .chd no longer matches what was recorded when it was made. Make a new one from your disc "
                     "if you can.";
        } else if (v.warning == SetupWarning::Unverified) {
            t.heading = "This .chd can't be checked";
            t.body = "It isn't damaged, but it was made in a way that can't be compared with the known good copy. It will "
                     "most likely work.";
            t.buttons = {"Choose another", "Use it"};
        }
        break;
    case SetupScreenView::Kind::Progress: {
        t.heading = v.heading.empty() ? "Getting Road Trip ready" : v.heading;
        char step[64] = "";
        if (v.steps > 1) std::snprintf(step, sizeof(step), "Step %d of %d: ", std::max(1, v.step), v.steps);
        t.phaseLine = step + v.phase;
        char pct[32];
        if (v.fraction >= 0.0) std::snprintf(pct, sizeof(pct), "%d%%", static_cast<int>(std::min(1.0, v.fraction) * 100.0));
        else std::snprintf(pct, sizeof(pct), "%d:%02d", v.elapsedSeconds / 60, v.elapsedSeconds % 60);
        t.percent = pct;
        if (v.fraction < 1.0 && v.etaSeconds >= 0.0) {
            const double s = v.etaSeconds;
            if (s < 10.0) t.eta = "Almost done";
            else if (s < 55.0) t.eta = "About " + std::to_string((static_cast<int>(s) + 5) / 10 * 10) + " seconds left";
            else {
                const int minutes = static_cast<int>(s / 60.0 + 0.5);
                t.eta = minutes <= 1 ? "About a minute left" : "About " + std::to_string(minutes) + " minutes left";
            }
        }
        if (t.eta.empty() && v.fraction < 0.0) t.eta = "Working...";
        t.footer = v.detail.empty() ? "This only happens once. You can switch to other apps meanwhile." : v.detail;
        if (v.skippable) t.buttons = {v.skipLabel};
        break;
    }
    }
    return t;
}

namespace {
// failure = real work failed (worth a visible Failed screen); a missing disc is not, the app runs on without it.
struct Attempt { bool ok = false; bool failure = false; std::string error; };

double secondsSince(std::chrono::steady_clock::time_point t) {
    return std::chrono::duration<double>(std::chrono::steady_clock::now() - t).count();
}

Attempt runAttempt(SetupHost* host) {
    Attempt result;
    const auto started = std::chrono::steady_clock::now();
    const auto assets = std::getenv("RT_ASSET_DIR") ? std::filesystem::path(std::getenv("RT_ASSET_DIR")) : dataRoot() / "assets";
    // Startup fast path: existence+size (see checkManifestFiles note above).
    // Full validation still runs once right after extraction and on demand
    // via RT_APP_SETUP_VERIFY=full.
    const char* verify = std::getenv("RT_APP_SETUP_VERIFY");
    if (checkManifestFiles(assets, verify && std::string(verify) == "full")) { log("assets: ready"); result.ok = true; return result; }
    std::filesystem::path disc;
    if (const char* image = std::getenv("RT_DISC"); image && *image && std::filesystem::is_regular_file(image)) {
        // RT_DISC names an .iso/.bin/.cue file: extract the needed tree (gated on the boot-ELF CRC inside
        // iso_extract) into <data>/disc, then continue exactly as for a pre-extracted directory.
        disc = dataRoot() / "disc";
        FirstRunExtraction flow;
        bool extracted = false;
        if (!host) extracted = flow.run(image, disc);
        else {
            // The flow blocks and is polled through snapshot(): run it aside so the host can keep drawing.
            std::atomic<bool> done{false};
            std::thread worker([&] { extracted = flow.run(image, disc); done = true; });
            while (!done) {
                const auto s = flow.snapshot();
                SetupScreenView view; view.kind = SetupScreenView::Kind::Progress;
                view.step = s.stepIndex + 1; view.steps = s.stepCount; view.fraction = s.fraction; view.etaSeconds = s.etaSeconds;
                view.elapsedSeconds = static_cast<int>(secondsSince(started));
                if (host->pump(view) == SetupInput::Close) { host->closed = true; flow.requestStop(); }
                std::this_thread::sleep_for(std::chrono::milliseconds(8));
            }
            worker.join();
            if (host->closed) return result;
        }
        if (!extracted) {
            SetupScreenView failed; failed.kind = SetupScreenView::Kind::Failed; failed.error = flow.snapshot().message;
            log("rom: " + flow.snapshot().message);
            log("setup: " + describeSetupScreen(failed).heading + " - " + describeSetupScreen(failed).body);
            result.failure = true; result.error = failed.error;
            return result;
        }
#ifdef _WIN32
        _putenv_s("RT_DISC", disc.string().c_str());
#else
        setenv("RT_DISC", disc.c_str(), 1);
#endif
    } else disc = findDisc();
    if (disc.empty() || !hasExpectedBootElfCrc(disc / "SLUS_203.98")) { log("rom: invalid"); return result; }

    std::vector<std::pair<std::filesystem::path, std::filesystem::path>> files;
    collect(disc, "SYS/FONTE.GSL", "fonts", files);
    collect(disc, "FLD", "textures/FLD", files);
    for (const char* dir : {"CAR0", "CAR1", "CAR2", "CAR3", "CAR4", "CAR5", "CARS"}) collect(disc, dir, (std::string("cars/") + dir).c_str(), files);
    collect(disc, "SOUND", "audio/SOUND", files);
    if (files.empty()) { log("rom: invalid"); return result; }

    SetupManifest manifest;
    auto lastPump = std::chrono::steady_clock::time_point{};
    for (std::size_t i = 0; i < files.size(); ++i) {
        if (host && (i + 1 == files.size() || secondsSince(lastPump) >= 0.033)) {
            lastPump = std::chrono::steady_clock::now();
            SetupScreenView view; view.kind = SetupScreenView::Kind::Progress;
            view.fraction = static_cast<double>(i) / static_cast<double>(files.size());
            view.elapsedSeconds = static_cast<int>(secondsSince(started));
            if (view.fraction >= 0.05) view.etaSeconds = secondsSince(started) * (1.0 - view.fraction) / view.fraction;
            if (host->pump(view) == SetupInput::Close) { host->closed = true; return result; }
        }
        const auto& [source, relative] = files[i]; const auto destination = assets / relative;
        std::error_code ec; std::filesystem::create_directories(destination.parent_path(), ec);
        const auto expectedSize = std::filesystem::file_size(source, ec); const auto expectedCrc = crcFile(source);
        if (!std::filesystem::is_regular_file(destination, ec) || std::filesystem::file_size(destination, ec) != expectedSize || crcFile(destination) != expectedCrc) {
            std::filesystem::copy_file(source, destination, std::filesystem::copy_options::overwrite_existing, ec);
            if (ec) { log("extract: failed " + relative.string()); result.failure = true; result.error = "Couldn't write " + relative.string() + "."; return result; }
        }
        manifest.files.push_back({relative.generic_string(), expectedSize, expectedCrc});
        const unsigned percent = static_cast<unsigned>((i + 1) * 100 / files.size());
        const char* kind = relative.string().find("fonts/") == 0 ? "FONTE" : relative.string().find("textures/") == 0 ? "terrain" : relative.string().find("cars/") == 0 ? "cars" : "audio";
        log(std::string("extract: ") + kind + " " + std::to_string(percent) + "%");
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    if (!writeSetupManifest(assets / "manifest.json", manifest) || !checkManifestFiles(assets, true)) {
        log("extract: manifest failed");
        result.failure = true; result.error = "The extracted files didn't check out.";
        return result;
    }
    log("assets: ready");
    result.ok = true;
    return result;
}
} // namespace

bool RunFirstRunSetupIfRequired(SetupHost* host) {
    for (;;) {
        const Attempt attempt = runAttempt(host);
        if (attempt.ok || !host || host->closed || !attempt.failure) return attempt.ok;
        SetupScreenView failed; failed.kind = SetupScreenView::Kind::Failed; failed.error = attempt.error;
        for (;;) {  // the failure stays on screen until the user acts
            const SetupInput input = host->pump(failed);
            if (input == SetupInput::Close) { host->closed = true; return false; }
            if (input == SetupInput::Continue) return false;
            if (input == SetupInput::TryAgain) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }
}
} // namespace rt::native::assets
