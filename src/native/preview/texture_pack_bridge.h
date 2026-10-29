#pragma once

// preview/texture_pack_bridge.h
//
// The renderers' half of the HD-pack seam. On a texture-cache MISS a backend
// asks here for the pixels to upload (the active pack's image when it has one
// for this content, the native texture otherwise) and, when dumping is on,
// writes the native texture once. Everything expensive lives on the miss path;
// a hit only mixes the pack generation into its cache key, so a switch cannot
// serve the previous pack's image.
//
// The hooks are a process-wide table with no-ops as the default, so a target
// that compiles preview_vk.cpp without the asset library (a seam test, a tool)
// still links: the app installs the real ones (assets/texture_pack.cpp) at
// startup.

#include "assets/png_decode.h"
#include "preview/preview.h"

#include <cstddef>
#include <cstdint>

namespace rt::native::preview {

struct PackHooks {
    std::uint64_t (*salt)() = nullptr;
    const assets::DecodedImage* (*replacement)(std::uint32_t width, std::uint32_t height,
                                               const std::uint8_t* rgba, std::size_t n) = nullptr;
    void (*dump)(std::uint32_t width, std::uint32_t height, const std::uint8_t* rgba,
                 std::size_t n) = nullptr;
};

inline PackHooks& packHooks() {
    static PackHooks hooks;
    return hooks;
}
inline void setPackHooks(const PackHooks& hooks) { packHooks() = hooks; }

// Cache-key salt: changes whenever the active pack changes.
inline std::uint64_t packKeySalt() { return packHooks().salt ? packHooks().salt() : 0; }

// The pack image for this decoded texture, or nullptr (native texture wins).
inline const assets::DecodedImage* packReplacement(const PreviewTexture& tex) {
    return packHooks().replacement
               ? packHooks().replacement(tex.width, tex.height, tex.rgba.data(), tex.rgba.size())
               : nullptr;
}

// Dumps the native texture if RT_TEXTURE_DUMP is on (once per content).
inline void packDump(const PreviewTexture& tex) {
    if (packHooks().dump) packHooks().dump(tex.width, tex.height, tex.rgba.data(), tex.rgba.size());
}

}  // namespace rt::native::preview
