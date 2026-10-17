#include "../../src/native/assets/title_screen.h"

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
    const auto paths = titleBackdropPaths("/disc");
    check(paths.size() == 1 && paths[0] == std::filesystem::path("/disc/SYS/TITLE.GSL"),
          "discovery path ordering");
    check(titleBackdropPaths({}).empty(), "empty root has no discovery paths");
    rt::native::preview::PreviewTexture bad{1, 1, {1, 2, 3, 4}};
    check(!decodeTitleBackdrop({}, bad) && bad.width == 0 && bad.rgba.empty(), "graceful malformed input");
    check(!loadTitleBackdrop("/definitely/not/a/disc").has_value(), "graceful missing disc");

    const auto title = disc() / "SYS/TITLE.GSL";
    if (!std::filesystem::is_regular_file(title)) {
        std::printf("SKIP (disc absent)\n");
        return failures ? 1 : 0;
    }
    const auto decoded = loadTitleBackdrop(disc());
    check(decoded.has_value() && decoded->width > 0 && decoded->height > 0, "runtime title decode");
    if (decoded) {
        const auto again = loadTitleBackdrop(disc());
        check(again && hash(*again) == hash(*decoded), "deterministic title hash");
        std::printf("title %ux%u fnv=%016llx\n", decoded->width, decoded->height,
                    static_cast<unsigned long long>(hash(*decoded)));
        const std::filesystem::path oracle = "build/scratch/oracle-golden/title.png";
        if (std::filesystem::is_regular_file(oracle)) {
            std::filesystem::create_directories("build/scratch/title-art");
            const std::filesystem::path ppm = "build/scratch/title-art/title-composed.ppm";
            std::ofstream image(ppm, std::ios::binary);
            image << "P6\n" << decoded->width << ' ' << decoded->height << "\n255\n";
            for (std::size_t i = 0; i < decoded->rgba.size(); i += 4)
                image.write(reinterpret_cast<const char*>(&decoded->rgba[i]), 3);
            image.close();
            const std::string command = "python3 scripts/frame_compare.py --oracle " + oracle.string() +
                                        " --native " + ppm.string() + " --label native-title-test";
            check(std::system(command.c_str()) == 0, "frame_compare title verdict");
        }
    }
    if (failures) return 1;
    std::printf("native_title_screen_test passed\n");
}
