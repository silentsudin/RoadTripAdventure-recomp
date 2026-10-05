#pragma once

// Extracts every texture the game uploads from the disc's files, named the way texture dumps
// and packs name them (see gs_texture_tools.cpp on the PS2Recomp fork): an FNV-1a hash of the
// decoded RGBA, as paraLLEl-GS decodes it. The game's files carry its GS upload packets
// (BITBLTBUF, TRXPOS, TRXREG, TRXDIR, then the pixels); each file's uploads are replayed into a
// GS memory image and decoded with the palette uploaded with them (or the one a TEX0 in the
// same file names).

#include <cstddef>
#include <filesystem>
#include <functional>
#include <string>

namespace rt::textures
{
    struct ExtractOptions
    {
        // TEXA for 16- and 24-bit colour (alpha of opaque texels, black-is-transparent).
        unsigned ta0 = 0x80, ta1 = 0x80;
        bool aem = true; // Road Trip sets TEXA.AEM (black 16/24-bit texels are transparent)
        // A 256-colour palette uploaded with a 16-colour texture: write all 16 banks (CSA)
        // instead of the first.
        bool allBanks = true;
    };

    struct ExtractStats
    {
        size_t files = 0;      // files with uploads
        size_t uploads = 0;    // image uploads replayed
        size_t textures = 0;   // textures decoded (with each palette)
        size_t written = 0;    // new PNGs
    };

    // Writes <hash>_<W>x<H>_psm<NN>.png for each texture into outDir (existing files are kept)
    // and outDir/sources.csv: where each texture came from.
    ExtractStats extractDisc(const std::filesystem::path &discDir, const std::filesystem::path &outDir,
                             const ExtractOptions &options = {},
                             const std::function<void(const std::string &)> &progress = {});
}
