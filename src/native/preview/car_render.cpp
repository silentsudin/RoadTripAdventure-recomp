#include "preview/car_render.h"

#include "assets/car_models.h"
#include "preview/car_mesh.h"

#include <array>
#include <cstdint>
#include <fstream>
#include <iterator>

namespace rt::native::preview {
namespace {

std::array<std::uint8_t, 32> provenanceDigest(std::span<const std::uint8_t> bytes) {
    // Provenance only rejects an all-zero digest. This deterministic digest is
    // deliberately not an identity claim; car_mesh.cpp verifies mesh source
    // bytes with SHA-256 before convertCarModels (car-data-status.md:250-258).
    std::array<std::uint8_t, 32> digest{};
    std::uint64_t h = 0xcbf29ce484222325ull;
    for (std::uint8_t byte : bytes) {
        h ^= byte;
        h *= 0x100000001b3ull;
    }
    for (std::size_t i = 0; i < digest.size(); ++i) {
        h ^= h << 13;
        h ^= h >> 7;
        h ^= h << 17;
        digest[i] = static_cast<std::uint8_t>(h);
    }
    return digest;
}

}  // namespace

bool loadCarRenderAsset(const std::string& carPath, CarRenderAsset& out,
                        std::string& error, float targetLength) {
    CarMeshResult mesh;
    if (!loadCarMeshFromFile(carPath, mesh, targetLength) ||
        mesh.model.vertices.size() < 3 || mesh.model.indices.size() < 3) {
        error = "CAR mesh decode failed";
        return false;
    }

    std::ifstream input(carPath, std::ios::binary);
    std::vector<std::uint8_t> bytes((std::istreambuf_iterator<char>(input)), {});
    if (!input || bytes.empty()) {
        error = "cannot read CAR livery";
        return false;
    }
    assets::Provenance provenance;
    provenance.sourceFile = carPath;
    provenance.sha256 = provenanceDigest(bytes);
    const assets::CarAssetResult decoded = assets::loadCarAssets(bytes, carPath, provenance.sha256);
    // Model 0's main VIF chain is followed by its two count-gated part chains.
    // Q00's 2264 + 82 + 30 decoded vertices match car_mesh's 2376-vertex body
    // merge; keep their authored ST values while car_mesh supplies its proven
    // body-part filtering and calibrated placement (car-data-status.md:71-81).
    if (decoded.err != assets::CarAssetError::Ok || decoded.models.size() < 3) {
        error = "CAR textured mesh decode failed";
        return false;
    }
    std::size_t uvVertex = 0;
    for (std::size_t model = 0; model < 3; ++model) {
        for (const auto& vertex : decoded.models[model].vertices) {
            if (uvVertex >= mesh.model.vertices.size()) {
                error = "CAR mesh/livery vertex layout mismatch";
                return false;
            }
            mesh.model.vertices[uvVertex].u = vertex.u;
            mesh.model.vertices[uvVertex].v = vertex.v;
            ++uvVertex;
        }
    }
    if (uvVertex != mesh.model.vertices.size()) {
        error = "CAR mesh/livery vertex count mismatch";
        return false;
    }
    const assets::CarLiveryResult livery = assets::extractCarLivery(bytes, provenance, "livery");
    if (livery.err != assets::CarLiveryError::Ok || livery.image.width != 128 ||
        livery.image.height != 128 || livery.image.pixels.size() != 128u * 128u * 4u) {
        error = "CAR livery decode failed";
        return false;
    }

    // Q00's longest axis is X, while previewRenderScene applies heading along
    // local Z. This is an axis convention bridge, not a format guess
    // (car-data-status.md:89-92; main's prior Q00 bridge).
    if (mesh.chosenAxis == "x") {
        for (auto& vertex : mesh.model.vertices) {
            const float x = vertex.px;
            vertex.px = -vertex.pz;
            vertex.pz = x;
            const float nx = vertex.nx;
            vertex.nx = -vertex.nz;
            vertex.nz = nx;
        }
    }
    out.model = std::move(mesh.model);
    out.livery = {livery.image.width, livery.image.height, livery.image.pixels};
    return true;
}

bool renderPlayerCarChase(Renderer* renderer, const CarRenderAsset& car,
                          const gameplay::FrameSnapshot& snapshot,
                          const gameplay::World& world,
                          std::vector<std::uint8_t>& rgbaOut, std::string& error,
                          const TerrainMesh* terrain, const std::vector<RouteLine>* routes) {
    if (car.model.indices.size() < 3 || car.livery.width == 0) {
        error = "empty player car render asset";
        return false;
    }
    // previewRenderScene's 3D texture pass is selected by a textured terrain.
    // The standalone chase preview has no field mesh, so provide a one-page
    // white ground that selects that established pass; a real terrain retains
    // ownership when supplied by the app.
    TerrainMesh texturedFallback;
    if (terrain == nullptr) {
        texturedFallback.positions = {{0.f, 0.f, 0.f}, {160.f, 0.f, 0.f},
                                     {160.f, 0.f, 160.f}, {0.f, 0.f, 160.f}};
        texturedFallback.indices = {0, 2, 1, 0, 3, 2};
        texturedFallback.colours.assign(4, {0.16f, 0.42f, 0.18f, 1.f});
        texturedFallback.uvs = {{0.f, 0.f}, {1.f, 0.f}, {1.f, 1.f}, {0.f, 1.f}};
        texturedFallback.textures = {{1, 1, {255, 255, 255, 255}}};
        texturedFallback.triangleTexture = {0, 0};
        terrain = &texturedFallback;
    }
    return previewRenderScene(renderer, car.model, snapshot, world, rgbaOut, error,
                              terrain, routes, &car.livery);
}

}  // namespace rt::native::preview
