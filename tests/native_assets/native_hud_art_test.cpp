#include "../../src/native/assets/hud_art.h"

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>

namespace {
int failures = 0;
void check(bool condition, const char* name) {
    if (!condition) { std::fprintf(stderr, "FAIL %s\n", name); ++failures; }
}
std::uint64_t hash(const rt::native::preview::PreviewTexture& texture) {
    std::uint64_t result = 14695981039346656037ull;
    for (const auto byte : texture.rgba) { result ^= byte; result *= 1099511628211ull; }
    return result;
}
std::filesystem::path disc() {
    if (const char* env = std::getenv("RT_DISC")) return env;
    if (const char* home = std::getenv("HOME"))
        return std::filesystem::path(home) / "Library/Application Support/RoadTripRecomp/disc";
    return {};
}
}

int main() {
    using namespace rt::native::assets;
    // discovery order (public for testability, disc-first)
    const auto mpaths = meterDialPaths("/disc");
    check(!mpaths.empty() && mpaths[0] == std::filesystem::path("/disc/ITEM/METER0.GSL"),
          "meter discovery path ordering");
    const auto spaths = scoreAtlasPaths("/disc");
    check(!spaths.empty() && spaths[0] == std::filesystem::path("/disc/SYS/SCORE.GSL"),
          "score atlas discovery path ordering");
    // packaged fallbacks mean {} may still yield paths (correct, tested via graceful load below)

    // graceful malformed / missing
    rt::native::preview::PreviewTexture bad{1, 1, {1, 2, 3, 4}};
    check(!decodeMeterDial({}, bad) && bad.width == 0 && bad.rgba.empty(), "graceful malformed meter input");
    check(!decodeRaceAtlas({}, bad) && bad.width == 0 && bad.rgba.empty(), "graceful malformed score input");
    check(!loadHudArt("/definitely/not/a/disc").speedDial.has_value() &&
          !loadHudArt("/definitely/not/a/disc").raceAtlas.has_value(), "graceful missing disc yields empty HudArt");

    const auto m0 = disc() / "ITEM" / "METER0.GSL";
    const auto sc = disc() / "SYS" / "SCORE.GSL";
    if (!std::filesystem::is_regular_file(m0) || !std::filesystem::is_regular_file(sc)) {
        std::printf("SKIP (disc absent)\n");
        return failures ? 1 : 0;
    }

    const auto hud = loadHudArt(disc());
    check(hud.speedDial.has_value() && hud.speedDial->width == 256 && hud.speedDial->height == 128,
          "runtime meter dial decode 256x128");
    check(hud.raceAtlas.has_value() && hud.raceAtlas->width == 256 && hud.raceAtlas->height == 256,
          "runtime score atlas decode 256x256");

    if (hud.speedDial) {
        const auto again = loadHudArt(disc());
        check(again.speedDial && hash(*again.speedDial) == hash(*hud.speedDial), "deterministic speedDial hash");
        std::printf("speedDial %ux%u fnv=%016llx\n", hud.speedDial->width, hud.speedDial->height,
                    static_cast<unsigned long long>(hash(*hud.speedDial)));
        // PIL-measured on decoded artifact (build/scratch/hud-art/METER0.GSL_00000070_256x128_psm13.png):
        // mean (145,163,76) -- yellowish dial face region (2026-10-17)
        const auto& d = *hud.speedDial;
        std::size_t n = d.width * d.height;
        std::uint32_t mr = 0, mg = 0, mb = 0;
        for (std::size_t i = 0; i < d.rgba.size(); i += 4) { mr += d.rgba[i]; mg += d.rgba[i+1]; mb += d.rgba[i+2]; }
        mr /= n; mg /= n; mb /= n;
        // sanity: roughly matches the PIL mean of the source art (within 30/255 tolerance for variant lighting)
        check(std::abs(static_cast<int>(mr) - 145) < 30 && std::abs(static_cast<int>(mg) - 163) < 30,
              "speedDial mean colour sanity vs PIL golden source");
    }
    if (hud.raceAtlas) {
        const auto again = loadHudArt(disc());
        check(again.raceAtlas && hash(*again.raceAtlas) == hash(*hud.raceAtlas), "deterministic raceAtlas hash");
        std::printf("raceAtlas %ux%u fnv=%016llx\n", hud.raceAtlas->width, hud.raceAtlas->height,
                    static_cast<unsigned long long>(hash(*hud.raceAtlas)));
        // PIL on decoded (SCORE.GSL_00000070_256x256_psm14.png): mean (89,50,85)
        const auto& a = *hud.raceAtlas;
        std::size_t n = a.width * a.height;
        std::uint32_t mr = 0, mg = 0, mb = 0;
        for (std::size_t i = 0; i < a.rgba.size(); i += 4) { mr += a.rgba[i]; mg += a.rgba[i+1]; mb += a.rgba[i+2]; }
        mr /= n; mg /= n; mb /= n;
        check(std::abs(static_cast<int>(mr) - 89) < 20 && std::abs(static_cast<int>(mg) - 50) < 20,
              "raceAtlas mean colour sanity vs PIL golden source (SCORE.GSL unit0)");
    }

    // dimensions sanity (dial roughly rectangular 2:1, atlas square)
    if (hud.speedDial) {
        check(hud.speedDial->width > hud.speedDial->height / 2 && hud.speedDial->width < hud.speedDial->height * 3,
              "dial dimensions roughly sane");
    }

    if (failures) return 1;
    std::printf("native_hud_art_test passed\n");
}
