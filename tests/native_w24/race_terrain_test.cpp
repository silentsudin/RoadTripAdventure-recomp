#include "render/race_terrain.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>

namespace {
int checks = 0;
int failures = 0;
void check(bool value, const char* name) {
    ++checks;
    if (!value) { ++failures; std::printf("FAIL %s\n", name); }
}
}  // namespace

int main() {
    using namespace rt::native::raceTerrain;
    const Mesh first = build(0, {});
    const Mesh second = build(0, {});
    if (std::getenv("RT_DISC") == nullptr) {
        std::puts("SKIP race_terrain: set RT_DISC to the user disc extract");
        return 0;
    }
    check(!first.vertices.empty(), "C00 has terrain vertices");
    check(first.indices.size() >= 3 && first.indices.size() % 3 == 0, "triangle list");
    check(first.textured(), "texture uploads and triangle bindings");
    check(first.vertices.size() == second.vertices.size() && first.indices == second.indices &&
              first.triangleTexture == second.triangleTexture,
          "deterministic build");
    bool finiteUv = true;
    for (const Vertex& vertex : first.vertices)
        finiteUv = finiteUv && std::isfinite(vertex.uv[0]) && std::isfinite(vertex.uv[1]);
    check(finiteUv, "finite raw STQ UVs");
    bool validTexture = !first.textures.empty();
    for (const TextureUpload& texture : first.textures)
        validTexture = validTexture && texture.width != 0 && texture.height != 0 &&
                       texture.rgba.size() == static_cast<std::size_t>(texture.width) * texture.height * 4;
    check(validTexture, "real disc texture uploads");
    std::printf("checks=%d failed=%d %s\n", checks, failures, first.diagnostic.c_str());
    return failures == 0 ? 0 : 1;
}
