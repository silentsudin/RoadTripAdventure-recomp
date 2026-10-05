// rt_texture_extract <disc dir> <output dir> [--ta0 N] [--ta1 N] [--no-aem] [--first-bank]
//
// Every texture on the disc as PNG, named like the app's texture dumps (textures/dumps), so
// edited copies work as a texture pack. The disc dir is the extracted disc in the data folder
// (e.g. ~/Library/Application Support/RoadTripRecomp/disc).

#include "textures/Extract.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::fprintf(stderr, "usage: %s <disc dir> <output dir> [--ta0 N] [--ta1 N] [--no-aem] [--first-bank]\n", argv[0]);
        return 2;
    }
    rt::textures::ExtractOptions options;
    for (int i = 3; i < argc; ++i)
    {
        if (!std::strcmp(argv[i], "--ta0") && i + 1 < argc)
            options.ta0 = unsigned(std::strtoul(argv[++i], nullptr, 0));
        else if (!std::strcmp(argv[i], "--ta1") && i + 1 < argc)
            options.ta1 = unsigned(std::strtoul(argv[++i], nullptr, 0));
        else if (!std::strcmp(argv[i], "--no-aem"))
            options.aem = false;
        else if (!std::strcmp(argv[i], "--first-bank"))
            options.allBanks = false;
    }
    const auto stats = rt::textures::extractDisc(argv[1], argv[2], options, [](const std::string &line) {
        std::fprintf(stderr, "%s\n", line.c_str());
    });
    std::printf("%zu files, %zu uploads, %zu textures decoded, %zu new PNGs\n", stats.files, stats.uploads, stats.textures,
                stats.written);
    return 0;
}
