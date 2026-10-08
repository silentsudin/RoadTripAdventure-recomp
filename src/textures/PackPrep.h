#pragma once

#include <cstdint>
#include <string>

// Converting a texture pack to its ASTC cache (gs_texture_pack_cache.h) from the menu: a pack
// chosen there is converted while the menu holds the game (never during play), with its progress
// in the Texture pack row; closing the menu stops it, and what is left converts at the next start.
namespace rt::textures::packprep
{
    // Converts `packDir` in the background if it has images without a current copy (another pack's
    // conversion stops first). Hardware GS only; "" stops.
    void want(const std::string &packDir);
    void stop();
    struct Status
    {
        bool checking = false; // looking for images to convert
        bool running = false;
        bool finished = false; // it ran to the end (this pack, this session)
        uint32_t done = 0, total = 0;
        double secondsLeft = -1.0; // from the speed so far (-1: not known yet)
    };
    Status status();
}
