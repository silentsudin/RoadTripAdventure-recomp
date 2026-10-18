#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace rt::native::raceTerrain {

// Render-space pose (Y is supplied by the terrain itself).  The current
// COURSE chains are static, so it selects the integration camera, not geometry.
struct Pose {
    float x = 0.f;
    float z = 0.f;
    float heading = 0.f;
};

struct Vertex {
    std::array<float, 3> position{};
    std::array<float, 2> uv{};
    std::array<float, 4> colour{};
};

struct TextureUpload {
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    std::vector<std::uint8_t> rgba;
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<std::uint32_t> indices;
    std::vector<TextureUpload> textures;
    // One texture index per triangle; -1 is intentionally the renderer's white fallback.
    std::vector<std::int32_t> triangleTexture;
    std::string diagnostic;

    [[nodiscard]] bool textured() const {
        return !vertices.empty() && !textures.empty() && indices.size() % 3 == 0 &&
               triangleTexture.size() == indices.size() / 3;
    }
};

// Reads $RT_DISC/COURSE/Cnn.BIN and exposes its decoded entry-4 terrain as a
// renderer-neutral triangle list.  The disc stays user supplied at run time.
Mesh build(int courseId, Pose pose = {});

}  // namespace rt::native::raceTerrain
