#pragma once

#include "assets/native_model.h"
#include "gameplay/game_loop.h"
#include "gameplay/world.h"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace rt::native::preview {

struct ModelStats {
    std::uint64_t triangles = 0;
    std::array<float, 3> bmin{};
    std::array<float, 3> bmax{};
};

// A solid, axis-aligned rectangle in target pixels. (x, y) is the top-left
// corner; positive y moves down the image.
struct ScreenQuad {
    float x = 0;
    float y = 0;
    float width = 0;
    float height = 0;
    std::array<float, 4> color{1, 1, 1, 1};
};

ModelStats computeStats(const assets::NativeModel& model);

// RGBA8 8-bit PNG (stored deflate blocks; no external dependency).
std::vector<std::uint8_t> encodePng(std::uint32_t width, std::uint32_t height,
                                    const std::vector<std::uint8_t>& rgba);

// Persistent offscreen Vulkan context (instance/device/pipeline/targets created once).
struct Renderer;
Renderer* previewInit(std::uint32_t width, std::uint32_t height, std::string& error,
                      std::string* deviceName);
// Re-uses the context; null view/proj => auto-fit camera. Geometry is re-uploaded per call.
bool previewRender(Renderer* r, const assets::NativeModel& model, const float* view, const float* proj,
                   std::vector<std::uint8_t>& rgbaOut, std::string& error);
// Optional real-terrain overlay for the scene path: y-up render-unit
// positions with a triangle list and one material id per triangle (used for
// colour banding until authentic textures bind).
// Textured path (optional): when `colours`, `uvs` and `textures` are populated the
// terrain draws through the textured pipeline instead of the banded-colour
// one.  `uvs` are GS page fractions (parallel to positions): s=1 spans one
// page, so REPEAT wrapping at integer bounds reproduces the hardware.
// `triangleTexture` gives one page id per triangle (index into `textures`,
// -1 = unresolved: drawn from an internal white dummy page).
struct PreviewTexture {
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    std::vector<std::uint8_t> rgba;  // width*height*4, R at byte 0
};

struct TerrainMesh {
    std::vector<std::array<float, 3>> positions;
    std::vector<std::uint32_t> indices;
    std::vector<std::uint32_t> triangleMaterials;
    std::vector<std::array<float, 4>> colours;             // optional, per position RGBA
    std::vector<std::array<float, 2>> uvs;          // optional, per position
    std::vector<PreviewTexture> textures;           // optional, page atlas
    std::vector<std::int32_t> triangleTexture;      // optional, per triangle
    bool isTextured() const {
        return !colours.empty() && !uvs.empty() && !textures.empty() &&
               colours.size() == positions.size() && uvs.size() == positions.size() &&
               triangleTexture.size() == indices.size() / 3;
    }
};

// Draws the authored field (grass and an explicit road ribbon) plus `car`, using
// the camera and time-of-day lighting emitted by GameLoop. When `terrain` is
// non-null it replaces the flat grass quad with real extracted terrain.
// `routes` (optional) draws nav-graph centerlines as flat colored strips just
// above the ground — the RT_APP_NAV=1 visualizer for the ROM route graphs
// (docs/native-reference/nav-tables.md); coordinates are render units.
struct RouteLine {
    float ax, az, bx, bz;
    float r, g, b;
};
bool previewRenderScene(Renderer* r, const assets::NativeModel& car,
                        const gameplay::FrameSnapshot& snapshot, const gameplay::World& world,
                        std::vector<std::uint8_t>& rgbaOut, std::string& error,
                        const TerrainMesh* terrain = nullptr,
                        const std::vector<RouteLine>* routes = nullptr);
// Draws solid screen-space rectangles over clearColor. Rectangle coordinates
// are target pixels with a top-left origin; this path has no lighting, depth,
// or texture sampling.
bool previewRender2D(Renderer* r, const std::vector<ScreenQuad>& quads,
                     const std::array<float, 4>& clearColor,
                     std::vector<std::uint8_t>& rgbaOut, std::string& error);

// HUD glyph pass (FONTE text). A HudQuad is a screen-space pixel rectangle
// (top-left origin) with atlas UVs and a modulate colour — build these from
// rt::native::assets::fonteSlotUv/fonteSlotForChar (see preview/race_hud.h
// for the shared builder).
struct HudQuad {
    float x = 0;
    float y = 0;
    float width = 0;
    float height = 0;
    float u0 = 0, v0 = 0, u1 = 0, v1 = 0;
    std::array<float, 4> color{1, 1, 1, 1};
};

// Uploads the glyph atlas (RGBA8, e.g. the decoded 512x384 FONTE atlas from
// assets/glyph_atlas.h) for previewRender2DTextured(). Idempotent per content
// (cached by hash). Returns false without side effects on upload failure.
bool previewUploadFontAtlas(Renderer* r, const PreviewTexture& atlas, std::string& error);

// Like previewRender2D but draws textured glyph quads (nearest sampling, no
// depth, no GPU blending — empty atlas texels are discarded in the fragment
// stage and the caller composites the returned RGBA over the scene, matching
// the existing HUD flow).
bool previewRender2DTextured(Renderer* r, const std::vector<HudQuad>& quads,
                             const std::array<float, 4>& clearColor,
                             std::vector<std::uint8_t>& rgbaOut, std::string& error);
void previewDestroy(Renderer* r);

// One-shot convenience wrappers (init -> render -> destroy).
// Offscreen Vulkan render (no window/swapchain) -> tightly packed RGBA8.
// Returns false and fills `error` on failure.
bool renderModel(const assets::NativeModel& model, std::uint32_t width, std::uint32_t height,
                 std::vector<std::uint8_t>& rgbaOut, std::string& error, std::string* deviceName);

// Same, but with an explicit camera: `view` and `proj` are column-major 4x4
// (16 floats each); the model is drawn as-is in world space (clip = proj*view*p,
// Vulkan clip conventions: y down, depth 0..1).
bool renderFrame(const assets::NativeModel& model, const float* view, const float* proj,
                 std::uint32_t width, std::uint32_t height, std::vector<std::uint8_t>& rgbaOut,
                 std::string& error, std::string* deviceName);

} // namespace rt::native::preview
