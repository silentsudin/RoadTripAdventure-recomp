#include "render/race_terrain.h"

#include "assets/field_converter.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <sstream>

namespace rt::native::raceTerrain {
namespace {

std::filesystem::path discRoot() {
    if (const char* root = std::getenv("RT_DISC"); root && *root) return root;
    if (const char* home = std::getenv("HOME"); home && *home)
        return std::filesystem::path(home) / "Library/Application Support/RoadTripRecomp/disc";
    return {};
}

std::filesystem::path coursePath(int courseId) {
    std::ostringstream name;
    name << 'C' << std::setw(2) << std::setfill('0') << courseId << ".BIN";
    return discRoot() / "COURSE" / name.str();
}

}  // namespace

Mesh build(int courseId, Pose) {
    Mesh out;
    // asset-map.md §Code bindings: course request type 0..17 maps to C00..C14,
    // C18..C20.  Do not pretend the absent C15/C17 files exist.
    if (courseId < 0 || courseId > 20 || courseId == 15 || courseId == 17) {
        out.diagnostic = "unsupported COURSE id";
        return out;
    }
    const std::filesystem::path path = coursePath(courseId);
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        out.diagnostic = "cannot open " + path.string();
        return out;
    }
    const std::vector<std::uint8_t> bytes((std::istreambuf_iterator<char>(in)),
                                          std::istreambuf_iterator<char>());

    // COURSE containers use the FLD offset-delimited layout (course-data-status.md
    // §§1,8-10).  The shared parser's WorldStrip expansion is pinned by
    // field-texture-rendering.md §§89-109: TRI_STRIP triples and raw STQ page UVs.
    auto terrain = assets::buildFieldTexturedTerrain(bytes, .1f);
    if (!terrain) {
        out.diagnostic = "COURSE terrain decode failed (field error " +
                         std::to_string(static_cast<int>(terrain.error())) + ")";
        return out;
    }
    out.vertices.reserve(terrain->patch.positions.size());
    for (std::size_t i = 0; i < terrain->patch.positions.size(); ++i) {
        out.vertices.push_back({terrain->patch.positions[i], terrain->patch.uvs[i],
                                terrain->patch.colours[i]});
    }
    out.indices = std::move(terrain->patch.indices);
    out.triangleTexture = std::move(terrain->patch.triangleTexture);
    out.textures.reserve(terrain->patch.pages.size());
    for (const auto& page : terrain->patch.pages)
        out.textures.push_back({page.width, page.height, page.rgba});
    std::ostringstream diagnostic;
    diagnostic << path.filename().string() << " triangles=" << out.indices.size() / 3
               << " pages=" << out.textures.size() << " clut=" << terrain->clutHits << '/'
               << terrain->clutMisses << " unresolved=" << terrain->unresolvedTriangles;
    out.diagnostic = diagnostic.str();
    return out;
}

}  // namespace rt::native::raceTerrain
