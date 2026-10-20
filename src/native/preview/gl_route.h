#pragma once
// W16.3: which preview draw classes the GL backend renders itself, and the pure vertex builders it
// shares with the contract tests. Header-only, no GL/Vulkan dependency.
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace rt::native::preview {

enum class DrawClass {
    Flat2D,       // previewRender2D: solid NDC quads (uiPipe)
    UiTextured,   // previewUploadFontAtlas + previewRender2DTextured (hudPipe: FONTE glyphs, backdrop, dial)
    FlatLit,      // previewRender: flat-lit model (pipe, derivative normals + fog)
    Scene,        // previewRenderScene: terrain + car + sky + overlay in one depth pass (pipe/texPipe/ovPipe)
};

// W16.4: the scene (terrain + car + sky + overlay in one depth pass) is on GL too; every class is implemented
// and a GL-backed Renderer never creates a Vulkan device.
constexpr bool glImplements(DrawClass) { return true; }

struct FlatVertex { float x, y, r, g, b, a; };           // 24 B, mirrors UiVertex in preview_vk.cpp
struct GlyphVertex { float x, y, u, v, r, g, b, a; };    // 32 B, mirrors HudVertex

// Pixel (top-left origin) -> Vulkan NDC (y down). The GL backend does NOT flip: its FBO row 0 is the
// image top, so glReadPixels yields the same top-down rows as the Vulkan copy (design doc §8c).
inline float ndcX(float x, std::size_t W) { return x * 2.0f / static_cast<float>(W) - 1.0f; }
inline float ndcY(float y, std::size_t H) { return y * 2.0f / static_cast<float>(H) - 1.0f; }

// Same winding/order as previewRender2D / previewRender2DTextured: {a, c, b, a, d, c}.
template <class Vertex>
inline void pushQuad(std::vector<Vertex>& out, const Vertex& a, const Vertex& b, const Vertex& c, const Vertex& d) {
    out.insert(out.end(), {a, c, b, a, d, c});
}

// ---- W16.4: textured scene pieces shared by both backends and the contract test.

// Page slot of a triangle's texture id: slot 0 is the 1x1 white dummy (-1 / unresolved / out of range),
// slot n+1 is terrain->textures[n]. Mirrors the bucketing in previewRenderScene.
inline std::size_t pageSlot(std::int32_t page, std::size_t textureCount) {
    if (page < -1 || static_cast<std::size_t>(page) >= textureCount) page = -1;
    return static_cast<std::size_t>(page + 1);
}

// Vulkan's texSampler: NEAREST min/mag, REPEAT on U/V, one mip level, no anisotropy. Page textures and the
// HUD/font atlas share it (the GL twin sets these per texture object).
enum class SamplerClass { PageTexture, FontAtlas };
struct SamplerDesc { bool nearest; bool repeat; bool mips; };
constexpr SamplerDesc samplerFor(SamplerClass) { return {true, true, false}; }

// Sky ramp band -> 6 UI-pipeline vertices {x,y,r,g,b,a}, triangulation {a,e,c,a,d,e} (drawSkyBands).
struct SkyBandGl { float y0, y1; float c0[4], c1[4]; };
inline void skyBandVertices(std::vector<float>& out, const SkyBandGl& b) {
    if (b.y1 <= b.y0) return;
    const auto v = [&](float x, float y, const float* c) { out.insert(out.end(), {x, y, c[0], c[1], c[2], c[3]}); };
    v(-1.f, b.y0, b.c0); v(1.f, b.y1, b.c1); v(1.f, b.y0, b.c0);
    v(-1.f, b.y0, b.c0); v(-1.f, b.y1, b.c1); v(1.f, b.y1, b.c1);
}

} // namespace rt::native::preview
