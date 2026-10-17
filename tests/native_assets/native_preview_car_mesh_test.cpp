// tests/native_assets/native_preview_car_mesh_test.cpp
//
// Unit and integration tests for the CAR->NativeModel mesh builder
// (preview/car_mesh). Uses hand-built MeshRecord fixtures for the
// merge/normal/scale math (CAR byte construction is impractical for
// synthetic coverage) and the real disc file when present.
//
// Failure accounting is plain counters (no assert()), NDEBUG-safe.
// Exits 0 on SKIP or pass.

#include "../../src/native/assets/container.h"
#include "../../src/native/assets/model_converter.h"
#include "../../src/native/preview/car_mesh.h"

#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using rt::native::assets::MeshRecord;
using rt::native::assets::NativeModel;
using rt::native::assets::Vertex;
using rt::native::preview::buildCarMeshFromCarFile;
using rt::native::preview::CarMeshResult;
using rt::native::preview::CarMeshTransformInfo;
using rt::native::preview::CarMeshDiagnostics;
using rt::native::preview::inspectCarMeshDiagnostics;
using rt::native::preview::dumpCarMeshDiagnostics;
using rt::native::preview::loadCarMeshFromFile;
using rt::native::preview::mergeAndNormalizeCarMeshes;

namespace {

int g_checks = 0;
int g_failures = 0;

void check(bool cond, const std::string& what) {
    ++g_checks;
    if (!cond) {
        ++g_failures;
        std::cerr << "FAIL: " << what << '\n';
    }
}

int finish() {
    if (g_failures > 0) {
        std::cerr << "native_preview_car_mesh_test: " << g_failures << " of "
                  << g_checks << " checks FAILED\n";
        return EXIT_FAILURE;
    }
    std::cout << "native_preview_car_mesh_test: " << g_checks << " checks passed\n";
    return EXIT_SUCCESS;
}

using Bytes = std::vector<std::uint8_t>;
using Digest = std::array<std::uint8_t, 32>;

// Simple FNV-1a 64 over the vertex payload (positions+normals+uv+color) for
// determinism check. Ignores indices order for this test.
std::uint64_t fnvHashVertices(const NativeModel& m) {
    std::uint64_t h = 0xcbf29ce484222325ull;
    for (const Vertex& v : m.vertices) {
        const float fs[8] = {v.px, v.py, v.pz, v.nx, v.ny, v.nz, v.u, v.v};
        for (float f : fs) {
            std::uint32_t u = std::bit_cast<std::uint32_t>(f);
            for (int b = 0; b < 4; ++b) {
                std::uint8_t by = static_cast<std::uint8_t>((u >> (b * 8)) & 0xff);
                h ^= by;
                h *= 0x100000001b3ull;
            }
        }
        h ^= v.r; h *= 0x100000001b3ull;
        h ^= v.g; h *= 0x100000001b3ull;
        h ^= v.b; h *= 0x100000001b3ull;
        h ^= v.a; h *= 0x100000001b3ull;
    }
    return h;
}

bool allFinite(const NativeModel& m) {
    for (const Vertex& v : m.vertices) {
        const float fs[8] = {v.px, v.py, v.pz, v.nx, v.ny, v.nz, v.u, v.v};
        for (float f : fs) {
            if (!std::isfinite(f)) return false;
        }
    }
    return true;
}

std::uint64_t countNonZeroNormals(const NativeModel& m) {
    std::uint64_t n = 0;
    for (const Vertex& v : m.vertices) {
        if (v.nx != 0.0f || v.ny != 0.0f || v.nz != 0.0f) ++n;
    }
    return n;
}

} // namespace

int main() {
    const std::string kDiscPath =
        "/Users/holzr/Library/Application Support/RoadTripRecomp/disc/CAR0/Q00.BIN";

    // -----------------------------------------------------------------------
    // Synthetic mesh-merge / normal / scale math on hand-built MeshRecords.
    // (No CAR bytes required; tests the core of mergeAndNormalizeCarMeshes.)
    // -----------------------------------------------------------------------
    {
        // Tiny L shape: two triangles sharing an edge, all normals zero in input.
        MeshRecord r0;
        r0.vertices = {
            Vertex{0.f,0.f,0.f, 0,0,0, 0,0, 0,0,0,0},
            Vertex{1.f,0.f,0.f, 0,0,0, 0,0, 0,0,0,0},
            Vertex{0.f,1.f,0.f, 0,0,0, 0,0, 0,0,0,0},
        };
        r0.indices = {0,1,2};

        MeshRecord r1;
        r1.vertices = {
            Vertex{1.f,0.f,0.f, 0,0,0, 0,0, 0,0,0,0},
            Vertex{1.f,1.f,0.f, 0,0,0, 0,0, 0,0,0,0},
            Vertex{0.f,1.f,0.f, 0,0,0, 0,0, 0,0,0,0},
        };
        r1.indices = {0,1,2};  // local indices

        std::vector<MeshRecord> recs{r0, r1};

        CarMeshTransformInfo info{};
        NativeModel merged = mergeAndNormalizeCarMeshes(recs, 2.0f, info);

        check(merged.vertices.size() == 6, "merge: 3+3 verts (no dedup, as expected)");
        check(merged.indices.size() == 6, "merge: 3+3 indices");
        check(allFinite(merged), "merge: all floats finite");
        check(countNonZeroNormals(merged) == 6, "normals: flat per-face produced (all 6 verts have non-zero n)");
        // After scale to target 2.0 on longest horiz (here x or z extent ~1), longest~1, scale=2
        // bbox x from 0..1 -> after center/scale to width 2: x -1..1 , y bottom 0
        check(std::abs(info.scale - 2.0f) < 1e-4f, "scale: target 2 on unit extent gives scale=2");
        check(info.axis == "x" || info.axis == "z", "axis: reports a horizontal axis");
        check(std::abs(merged.vertices[0].px + 1.0f) < 0.01f ||
              std::abs(merged.vertices[0].px - 1.0f) < 0.01f, "center: x origin after align");
        check(merged.vertices[0].py >= -1e-5f, "y-bottom: min y at or above 0");
        // uv zeroed, neutral color
        check(merged.vertices[0].u == 0.0f && merged.vertices[0].v == 0.0f, "uv: zeroed");
        check(merged.vertices[0].r == 0xA0 && merged.vertices[0].g == 0xB8, "color: neutral body (light blue-grey)");
    }

    // -----------------------------------------------------------------------
    // Real disc file path (when present): full build + load path, counts,
    // scale, finiteness, determinism.
    // -----------------------------------------------------------------------
    {
        if (!std::filesystem::exists(kDiscPath)) {
            std::cout << "SKIP (disc absent)\n";
            return EXIT_SUCCESS;
        }

        std::cout << "native_preview_car_mesh_test: using " << kDiscPath << "\n";

        // Via bytes
        std::ifstream f(kDiscPath, std::ios::binary | std::ios::ate);
        check(!!f, "open real Q00.BIN");
        auto sz = static_cast<std::size_t>(f.tellg());
        f.seekg(0);
        Bytes bytes(sz);
        if (sz) f.read(reinterpret_cast<char*>(bytes.data()), static_cast<std::streamsize>(sz));
        check(!!f && bytes.size() == sz, "read full bytes");

        CarMeshResult r1 = buildCarMeshFromCarFile(bytes, kDiscPath, 4.0f);
        check(r1.err == rt::native::assets::CarModelError::Ok, "build: Ok on real Q00");
        check(r1.model.vertices.size() > 100, "build: merged verts >100 (model 0 body)");
        check(allFinite(r1.model), "build: all finite");
        const float longest = std::max(r1.bboxMax[0] - r1.bboxMin[0],
                                       r1.bboxMax[2] - r1.bboxMin[2]);
        const float target = 4.0f;
        check(std::abs(longest - target) <= 0.01f * target, "scale: longest horiz axis within 1% of target 4.0");
        check(r1.chosenAxis == "x" || r1.chosenAxis == "z", "axis: logged horizontal");
        check(r1.appliedScale > 0.0f && std::isfinite(r1.appliedScale), "scale: finite positive");
        check(r1.bboxMin[1] <= 1e-4f, "align: y bottom near 0");
        check(std::abs(r1.bboxMin[0] + r1.bboxMax[0]) < 1e-2f, "align: x center near 0");
        check(std::abs(r1.bboxMin[2] + r1.bboxMax[2]) < 1e-2f, "align: z center near 0");

        // Q00 source diagnostics make part filtering auditable. The selected
        // body must not retain a count-gated part whose source-Y (CAR up)
        // extent exceeds the main body's vertical extent by 1.5x.
        const CarMeshDiagnostics diag = inspectCarMeshDiagnostics(bytes, kDiscPath);
        check(diag.err == rt::native::assets::CarModelError::Ok,
              "diagnostics: decode all six models");
        dumpCarMeshDiagnostics(diag, std::cout);
        const float mainHeight = diag.models[0].main.max[1] - diag.models[0].main.min[1];
        const float acceptedHeight = diag.models[0].acceptedParts.max[1] -
                                     diag.models[0].acceptedParts.min[1];
        check(diag.models[0].acceptedParts.vertexCount == 0 ||
              acceptedHeight <= 1.5f * mainHeight + 1e-4f,
              "parts: accepted model-0 parts are no taller than 1.5x the body in source Y");

        const float length = std::max(r1.bboxMax[0] - r1.bboxMin[0],
                                      r1.bboxMax[2] - r1.bboxMin[2]);
        const float height = r1.bboxMax[1] - r1.bboxMin[1];
        const float width = std::min(r1.bboxMax[0] - r1.bboxMin[0],
                                     r1.bboxMax[2] - r1.bboxMin[2]);
        check(height / length >= 0.30f && height / length <= 0.35f,
              "proportions: post-align height is 0.30..0.35 of length");
        check(width / length >= 0.50f && width / length <= 0.70f,
              "proportions: post-align width is 0.50..0.70 of length");

        // Determinism: same bytes twice -> identical vertex payload hash
        CarMeshResult r2 = buildCarMeshFromCarFile(bytes, kDiscPath, 4.0f);
        check(r2.err == r1.err, "det: err same");
        check(r2.model.vertices.size() == r1.model.vertices.size(), "det: vertex count same");
        std::uint64_t h1 = fnvHashVertices(r1.model);
        std::uint64_t h2 = fnvHashVertices(r2.model);
        check(h1 == h2, "det: FNV hash of vertices identical on rerun");

        // Via loadCarMeshFromFile
        CarMeshResult r3{};
        bool ok = loadCarMeshFromFile(kDiscPath, r3, 4.0f, true);
        check(ok && r3.err == rt::native::assets::CarModelError::Ok, "load: succeeds and Ok");
        check(r3.model.vertices.size() == r1.model.vertices.size(), "load: same merged count as build");

        // Evidence for report (real Q00 model 0 merged)
        std::cout << "native_preview_car_mesh_test: real Q00 evidence: "
                  << r1.model.vertices.size() << " verts, "
                  << r1.model.indices.size() / 3 << " tris, "
                  << "scale=" << r1.appliedScale << " axis=" << r1.chosenAxis
                  << " bbox=[" << r1.bboxMin[0] << "," << r1.bboxMin[1] << "," << r1.bboxMin[2]
                  << " .. " << r1.bboxMax[0] << "," << r1.bboxMax[1] << "," << r1.bboxMax[2] << "]\n";
    }

    return finish();
}
