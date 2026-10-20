#pragma once
// W16.3: backend dispatch seam. preview_vk.cpp owns the public previewInit/previewRender* entry points; when
// the GL backend is registered (rt-app only, after the GL context exists) and selected, they forward to
// these ops. Tools that link only preview_vk.cpp never register one and stay Vulkan-only.
#include "preview/preview.h"

namespace rt::native::preview {

// W16.4: one scene frame (previewRenderScene): sky bands, flat pipe, textured pipe (page-batched ranges), then the
// depth-off overlay, in one depth pass. Vertex blocks are SceneVertex {xyz,rgb} (24 B) and TexturedVertex
// {xyz,rgb,uv} (32 B). `ranges[i].tex` points at the page image (cached by content hash on the GL side).
struct GlSceneRange { const PreviewTexture* tex; std::uint32_t first, count; };
struct GlSceneDraw {
    const float* flatV = nullptr; std::size_t nFlatV = 0; const std::uint32_t* flatI = nullptr; std::size_t nFlatI = 0;
    const float* texV = nullptr; std::size_t nTexV = 0; const std::uint32_t* texI = nullptr; std::size_t nTexI = 0;
    const GlSceneRange* ranges = nullptr; std::size_t nRanges = 0;
    const float* overlayV = nullptr; std::size_t nOverlayV = 0; const std::uint32_t* overlayI = nullptr; std::size_t nOverlayI = 0;
    const float* skyVerts = nullptr; std::size_t nSkyVerts = 0;  // skyBandVertices() output, 6 floats per vertex
    const float* pc36 = nullptr;                                 // mvp(16) view(16) fog(4), Vulkan clip space
    float clear[4] = {0, 0, 0, 1};
};

struct GlBackendOps {
    void* (*create)(std::uint32_t width, std::uint32_t height, std::string& error);
    void (*destroy)(void* gl);
    bool (*render2D)(void* gl, const std::vector<ScreenQuad>& quads, const std::array<float, 4>& clear,
                     std::vector<std::uint8_t>& rgbaOut, std::string& error);
    bool (*uploadAtlas)(void* gl, const PreviewTexture& atlas, std::string& error);
    bool (*render2DTextured)(void* gl, const std::vector<HudQuad>& quads, const std::array<float, 4>& clear,
                             std::vector<std::uint8_t>& rgbaOut, std::string& error);
    // Flat-lit pass: `verts` = nVerts x {x,y,z,r,g,b}; pc = mvp(16) view(16) fog(4), column-major, Vulkan clip space.
    bool (*renderLit)(void* gl, const float* verts, std::size_t nVerts, const std::uint32_t* indices,
                      std::size_t nIndices, const float* pc36, const float clear[4],
                      std::vector<std::uint8_t>& rgbaOut, std::string& error);
    bool (*renderScene)(void* gl, const GlSceneDraw& draw, std::vector<std::uint8_t>& rgbaOut, std::string& error);
};

// Defined in preview_vk.cpp: VkDevices created by this process (stays 0 in RT_APP_RENDERER=gl mode).
int previewVulkanDevicesCreated();
// Defined in preview_vk.cpp. Pass nullptr to unregister.
void registerGlBackend(const GlBackendOps* ops);
// Defined in preview_gl.cpp (rt-app only).
const GlBackendOps* glBackendOps();

} // namespace rt::native::preview
