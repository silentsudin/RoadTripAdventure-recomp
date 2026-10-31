#pragma once
// CRT filter for the composited frame (issue #21, Options > Graphics > CRT
// filter): scanlines and an aperture-grille mask, applied in place to the RGBA8
// picture the presenter is about to show, so Vulkan and GL see the same look.
// Static per-pixel gains: no history, no blur (the GPU pass with a real
// scanline blur lands with the composite wave, see w14-gpu-post-plan.md).

#include <cstdint>
#include <vector>

namespace rt::native::preview {

// In-place. Does nothing for a non-positive size or a buffer shorter than
// width*height*4. Alpha is left as it is.
void applyCrtFilter(std::vector<std::uint8_t>& rgba, int width, int height);

}  // namespace rt::native::preview
