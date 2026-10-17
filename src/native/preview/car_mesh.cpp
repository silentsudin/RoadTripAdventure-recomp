#include "preview/car_mesh.h"

#include "assets/container.h"
#include "assets/model_converter.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <ostream>
#include <span>
#include <string>
#include <vector>

namespace rt::native::preview {
namespace {

// ---------------------------------------------------------------------------
// Local helpers (duplicated minimal from model_converter.cpp / container.cpp
// to stay self-contained; no new public surface).
// ---------------------------------------------------------------------------

std::uint32_t readU32LE(const std::uint8_t* p) {
    return static_cast<std::uint32_t>(p[0]) |
           (static_cast<std::uint32_t>(p[1]) << 8) |
           (static_cast<std::uint32_t>(p[2]) << 16) |
           (static_cast<std::uint32_t>(p[3]) << 24);
}

bool isAllZero(const std::array<std::uint8_t, 32>& d) {
    return std::all_of(d.begin(), d.end(), [](std::uint8_t b){ return b == 0; });
}

std::uint32_t rotr32(std::uint32_t x, std::uint32_t n) {
    return (x >> n) | (x << (32 - n));
}

// Real SHA-256 (public-domain style compact implementation, not the
// placeholder FNV used by some tests/tools). Required so provenance is
// never the all-zero digest that convertCarModels rejects.
void sha256Transform(std::uint32_t state[8], const std::uint8_t data[64]) {
    static const std::uint32_t k[64] = {
        0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
        0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
        0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
        0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
        0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
        0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
        0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
        0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
    };
    std::uint32_t w[64];
    for (int i = 0; i < 16; ++i) {
        w[i] = (std::uint32_t(data[i * 4]) << 24) |
               (std::uint32_t(data[i * 4 + 1]) << 16) |
               (std::uint32_t(data[i * 4 + 2]) << 8) |
               std::uint32_t(data[i * 4 + 3]);
    }
    for (int i = 16; i < 64; ++i) {
        std::uint32_t s0 = rotr32(w[i - 15], 7) ^ rotr32(w[i - 15], 18) ^ (w[i - 15] >> 3);
        std::uint32_t s1 = rotr32(w[i - 2], 17) ^ rotr32(w[i - 2], 19) ^ (w[i - 2] >> 10);
        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }
    std::uint32_t a = state[0], b = state[1], c = state[2], d = state[3];
    std::uint32_t e = state[4], f = state[5], g = state[6], h = state[7];
    for (int i = 0; i < 64; ++i) {
        std::uint32_t S1 = rotr32(e, 6) ^ rotr32(e, 11) ^ rotr32(e, 25);
        std::uint32_t ch = (e & f) ^ ((~e) & g);
        std::uint32_t t1 = h + S1 + ch + k[i] + w[i];
        std::uint32_t S0 = rotr32(a, 2) ^ rotr32(a, 13) ^ rotr32(a, 22);
        std::uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
        std::uint32_t t2 = S0 + maj;
        h = g; g = f; f = e; e = d + t1;
        d = c; c = b; b = a; a = t1 + t2;
    }
    state[0] += a; state[1] += b; state[2] += c; state[3] += d;
    state[4] += e; state[5] += f; state[6] += g; state[7] += h;
}

std::array<std::uint8_t, 32> computeSha256(std::span<const std::uint8_t> data) {
    std::uint32_t state[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };
    std::uint64_t bitlen = static_cast<std::uint64_t>(data.size()) * 8;
    std::uint8_t buf[64] = {};
    std::size_t datalen = 0;

    auto transform = [&](const std::uint8_t* chunk) {
        sha256Transform(state, chunk);
    };

    for (std::size_t i = 0; i < data.size(); ++i) {
        buf[datalen++] = data[i];
        if (datalen == 64) {
            transform(buf);
            datalen = 0;
        }
    }

    // Padding
    std::size_t i = datalen;
    buf[i++] = 0x80;
    if (i > 56) {
        while (i < 64) buf[i++] = 0;
        transform(buf);
        i = 0;
    }
    while (i < 56) buf[i++] = 0;
    for (int j = 0; j < 8; ++j) {
        buf[63 - j] = static_cast<std::uint8_t>((bitlen >> (j * 8)) & 0xff);
    }
    transform(buf);

    std::array<std::uint8_t, 32> out{};
    for (int j = 0; j < 8; ++j) {
        out[j * 4 + 0] = static_cast<std::uint8_t>((state[j] >> 24) & 0xff);
        out[j * 4 + 1] = static_cast<std::uint8_t>((state[j] >> 16) & 0xff);
        out[j * 4 + 2] = static_cast<std::uint8_t>((state[j] >> 8) & 0xff);
        out[j * 4 + 3] = static_cast<std::uint8_t>(state[j] & 0xff);
    }
    return out;
}

void computeFlatNormals(assets::NativeModel& m) {
    const std::size_t n = m.indices.size();
    if (n % 3 != 0 || m.vertices.empty()) return;

    for (std::size_t t = 0; t + 2 < n; t += 3) {
        std::uint32_t i0 = m.indices[t];
        std::uint32_t i1 = m.indices[t + 1];
        std::uint32_t i2 = m.indices[t + 2];
        if (i0 >= m.vertices.size() || i1 >= m.vertices.size() || i2 >= m.vertices.size()) continue;

        const float p0[3] = { m.vertices[i0].px, m.vertices[i0].py, m.vertices[i0].pz };
        const float p1[3] = { m.vertices[i1].px, m.vertices[i1].py, m.vertices[i1].pz };
        const float p2[3] = { m.vertices[i2].px, m.vertices[i2].py, m.vertices[i2].pz };

        float ex = p1[0] - p0[0], ey = p1[1] - p0[1], ez = p1[2] - p0[2];
        float fx = p2[0] - p0[0], fy = p2[1] - p0[1], fz = p2[2] - p0[2];

        float nx = ey * fz - ez * fy;
        float ny = ez * fx - ex * fz;
        float nz = ex * fy - ey * fx;

        float len2 = nx * nx + ny * ny + nz * nz;
        if (len2 > 1e-12f) {
            float inv = 1.0f / std::sqrt(len2);
            nx *= inv; ny *= inv; nz *= inv;
        } else {
            nx = 0.0f; ny = 1.0f; nz = 0.0f;
        }

        // Flat per-face: assign to the three verts for this triangle (deterministic
        // in index buffer order; shared verts get the normal of the last face that
        // references them).
        m.vertices[i0].nx = nx; m.vertices[i0].ny = ny; m.vertices[i0].nz = nz;
        m.vertices[i1].nx = nx; m.vertices[i1].ny = ny; m.vertices[i1].nz = nz;
        m.vertices[i2].nx = nx; m.vertices[i2].ny = ny; m.vertices[i2].nz = nz;
    }
}

std::pair<std::array<float, 3>, std::array<float, 3>> computeBbox(const assets::NativeModel& m) {
    constexpr float inf = std::numeric_limits<float>::infinity();
    std::array<float, 3> bmin = {inf, inf, inf};
    std::array<float, 3> bmax = {-inf, -inf, -inf};
    for (const auto& v : m.vertices) {
        const float p[3] = {v.px, v.py, v.pz};
        for (int i = 0; i < 3; ++i) {
            bmin[i] = std::min(bmin[i], p[i]);
            bmax[i] = std::max(bmax[i], p[i]);
        }
    }
    if (m.vertices.empty()) {
        bmin = bmax = {0.0f, 0.0f, 0.0f};
    }
    return {bmin, bmax};
}

CarMeshBounds computeRecordBounds(std::span<const assets::MeshRecord* const> records) {
    constexpr float inf = std::numeric_limits<float>::infinity();
    CarMeshBounds bounds;
    bounds.min = {inf, inf, inf};
    bounds.max = {-inf, -inf, -inf};
    for (const assets::MeshRecord* record : records) {
        if (record == nullptr) continue;
        for (const auto& vertex : record->vertices) {
            const float p[3] = {vertex.px, vertex.py, vertex.pz};
            for (int axis = 0; axis < 3; ++axis) {
                bounds.min[axis] = std::min(bounds.min[axis], p[axis]);
                bounds.max[axis] = std::max(bounds.max[axis], p[axis]);
            }
            ++bounds.vertexCount;
        }
    }
    if (bounds.vertexCount == 0) {
        bounds.min = {0.0f, 0.0f, 0.0f};
        bounds.max = bounds.min;
    }
    return bounds;
}

float extent(const CarMeshBounds& bounds, int axis) {
    return bounds.max[axis] - bounds.min[axis];
}

struct ModelChains {
    const assets::MeshRecord* main = nullptr;
    std::array<const assets::MeshRecord*, 2> parts{};
};

struct CarChains {
    std::array<ModelChains, 6> models{};
};

bool readCarChains(std::span<const std::uint8_t> bytes,
                   const std::vector<assets::MeshRecord>& records,
                   CarChains& out) {
    if (bytes.size() < 0x30) return false;

    std::uint32_t word[7] = {};
    for (int i = 0; i < 7; ++i) {
        word[i] = readU32LE(bytes.data() + static_cast<std::size_t>(i) * 4);
    }
    for (std::size_t model = 0; model < out.models.size(); ++model) {
        const std::uint64_t modelStart = word[model];
        const std::uint64_t modelEnd = word[model + 1];
        if (modelStart + 0x10 > modelEnd || modelEnd > bytes.size()) return false;

        const std::uint64_t mainStart = modelStart + 0x10;
        const std::uint32_t partOffset[2] = {
            readU32LE(bytes.data() + static_cast<std::size_t>(modelStart)),
            readU32LE(bytes.data() + static_cast<std::size_t>(modelStart) + 4),
        };
        const std::uint32_t partCount[2] = {
            readU32LE(bytes.data() + static_cast<std::size_t>(modelStart) + 8),
            readU32LE(bytes.data() + static_cast<std::size_t>(modelStart) + 12),
        };
        for (const auto& record : records) {
            if (record.provenance.offset == mainStart) {
                out.models[model].main = &record;
            }
            for (std::size_t part = 0; part < 2; ++part) {
                const std::uint64_t partStart = modelStart + (partOffset[part] & ~0x0fu);
                if (partCount[part] != 0 && record.provenance.offset == partStart) {
                    out.models[model].parts[part] = &record;
                }
            }
        }
    }
    return true;
}

bool keepPart(const assets::MeshRecord* part, const CarMeshBounds& mainBounds) {
    if (part == nullptr || part->vertices.empty() || mainBounds.vertexCount == 0) return false;
    const std::array<const assets::MeshRecord*, 1> onePart = {part};
    const CarMeshBounds partBounds = computeRecordBounds(onePart);
    // Source Y is the CAR vertical lane. A count word is a draw gate, not a
    // proof that a chain belongs to the near body (model_converter.h:26-37).
    return extent(partBounds, 1) <= 1.5f * extent(mainBounds, 1);
}

std::vector<assets::MeshRecord> selectedBodyRecords(const ModelChains& chains) {
    std::vector<assets::MeshRecord> selected;
    if (chains.main == nullptr || chains.main->vertices.empty() || chains.main->indices.empty()) {
        return selected;
    }
    selected.push_back(*chains.main);
    const std::array<const assets::MeshRecord*, 1> main = {chains.main};
    const CarMeshBounds mainBounds = computeRecordBounds(main);
    for (const assets::MeshRecord* part : chains.parts) {
        if (part != nullptr && !part->indices.empty() && keepPart(part, mainBounds)) {
            selected.push_back(*part);
        }
    }
    return selected;
}

void applyScaleAndCenter(assets::NativeModel& m, float targetLength,
                         float& outScale, std::string& outAxis,
                         std::array<float, 3>& outBmin, std::array<float, 3>& outBmax) {
    auto [bmin, bmax] = computeBbox(m);
    float dx = bmax[0] - bmin[0];
    float dy = bmax[1] - bmin[1];
    float dz = bmax[2] - bmin[2];
    (void)dy; // y is never the scale basis

    bool useX = (dx >= dz);
    float longest = useX ? dx : dz;
    outAxis = useX ? "x" : "z";
    if (longest <= 0.0f) longest = 1.0f;
    float scale = targetLength / longest;
    outScale = scale;

    float cx = 0.5f * (bmin[0] + bmax[0]);
    float cz = 0.5f * (bmin[2] + bmax[2]);
    float by = bmin[1];
    // Q00's decoded near body is 1.470 source-Y tall for a 2.767 source-Z
    // length. The native chase calibration calls for a 1.1..1.4 RU body at a
    // 4 RU horizontal length, so preserve the 4 RU horizontal convention and
    // apply the explicit 0.61 host vertical calibration (2.124 * .61 = 1.296
    // RU). This is a host-display calibration, not a recovered VIF transform.
    constexpr float kVerticalCalibration = 0.61f;

    for (auto& v : m.vertices) {
        v.px = (v.px - cx) * scale;
        v.pz = (v.pz - cz) * scale;
        v.py = (v.py - by) * scale * kVerticalCalibration;
    }

    // Final (post-align) bbox: x/z symmetric around 0, y bottom at 0
    float halfx = dx * 0.5f * scale;
    float halfz = dz * 0.5f * scale;
    float h = (bmax[1] - bmin[1]) * scale * kVerticalCalibration;
    outBmin = {-halfx, 0.0f, -halfz};
    outBmax = { halfx, h,  halfz};
}

// Neutral body color for the merged car (light blue-grey). The original CAR
// vertex streams carry no reliable per-vertex body colour for the main hull
// (many are zero or livery-modulated); this is UNDOCUMENTED in the model
// format and chosen for preview silhouette until livery wiring.
constexpr std::uint8_t kNeutralR = 0xA0;
constexpr std::uint8_t kNeutralG = 0xB8;
constexpr std::uint8_t kNeutralB = 0xD0;
constexpr std::uint8_t kNeutralA = 0xFF;

} // namespace

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

assets::NativeModel mergeAndNormalizeCarMeshes(
    std::span<const assets::MeshRecord> records,
    float targetLength,
    CarMeshTransformInfo& info) {

    assets::NativeModel model;
    std::vector<assets::Vertex> verts;
    std::vector<std::uint32_t> inds;
    verts.reserve(4096);
    inds.reserve(8192);

    for (const assets::MeshRecord& rec : records) {
        if (rec.vertices.empty() || rec.indices.empty()) continue;
        const std::uint32_t base = static_cast<std::uint32_t>(verts.size());
        verts.insert(verts.end(), rec.vertices.begin(), rec.vertices.end());
        for (std::uint32_t idx : rec.indices) {
            inds.push_back(base + idx);
        }
    }

    model.vertices = std::move(verts);
    model.indices = std::move(inds);

    // Apply neutral colour + uv=0 to every vertex (overwrites whatever the
    // MeshRecord carried from the VIF decode).
    for (auto& v : model.vertices) {
        v.u = 0.0f;
        v.v = 0.0f;
        v.r = kNeutralR;
        v.g = kNeutralG;
        v.b = kNeutralB;
        v.a = kNeutralA;
    }

    // Scale/center using longest horizontal axis. Also fills info.
    applyScaleAndCenter(model, targetLength, info.scale, info.axis,
                        info.bboxMin, info.bboxMax);

    // The calibrated vertical scale is non-uniform, so derive normals after
    // the transform rather than retaining normals from source-space faces.
    computeFlatNormals(model);
    return model;
}

CarMeshResult buildCarMeshFromCarFile(std::span<const std::uint8_t> bytes,
                                      const std::string& sourceFile,
                                      float targetLength) {
    CarMeshResult res;
    res.sourceFile = sourceFile;

    if (bytes.size() < 0x20) {
        res.err = assets::CarModelError::Truncated;
        return res;
    }

    const auto sha = computeSha256(bytes);
    if (isAllZero(sha)) {
        // Should never happen for real data; convert would reject anyway.
        res.err = assets::CarModelError::MissingProvenance;
        return res;
    }

    const auto cres = assets::convertCarModels(bytes, sourceFile, sha);
    if (cres.err != assets::CarModelError::Ok) {
        res.err = cres.err;
        return res;
    }

    CarChains chains;
    if (!readCarChains(bytes, cres.value.meshes, chains)) {
        res.err = assets::CarModelError::BadHeader;
        return res;
    }
    std::vector<assets::MeshRecord> model0 = selectedBodyRecords(chains.models[0]);
    if (model0.empty()) {
        res.err = assets::CarModelError::BadBatch;
        return res;
    }

    CarMeshTransformInfo info;
    res.model = mergeAndNormalizeCarMeshes(model0, targetLength, info);
    res.appliedScale = info.scale;
    res.bboxMin = info.bboxMin;
    res.bboxMax = info.bboxMax;
    res.chosenAxis = info.axis;
    res.err = assets::CarModelError::Ok;
    return res;
}

CarMeshDiagnostics inspectCarMeshDiagnostics(std::span<const std::uint8_t> bytes,
                                             const std::string& sourceFile) {
    CarMeshDiagnostics diagnostics;
    if (bytes.size() < 0x30) {
        diagnostics.err = assets::CarModelError::Truncated;
        return diagnostics;
    }
    const auto sha = computeSha256(bytes);
    if (isAllZero(sha)) {
        diagnostics.err = assets::CarModelError::MissingProvenance;
        return diagnostics;
    }
    const auto converted = assets::convertCarModels(bytes, sourceFile, sha);
    if (converted.err != assets::CarModelError::Ok) {
        diagnostics.err = converted.err;
        return diagnostics;
    }
    CarChains chains;
    if (!readCarChains(bytes, converted.value.meshes, chains)) {
        diagnostics.err = assets::CarModelError::BadHeader;
        return diagnostics;
    }
    for (std::size_t model = 0; model < chains.models.size(); ++model) {
        const ModelChains& modelChains = chains.models[model];
        std::vector<const assets::MeshRecord*> raw;
        std::vector<const assets::MeshRecord*> accepted;
        std::vector<const assets::MeshRecord*> rejected;
        if (modelChains.main != nullptr) raw.push_back(modelChains.main);
        if (modelChains.main != nullptr) accepted.push_back(modelChains.main);
        const CarMeshBounds mainBounds = computeRecordBounds(accepted);
        for (const assets::MeshRecord* part : modelChains.parts) {
            if (part == nullptr) continue;
            raw.push_back(part);
            (keepPart(part, mainBounds) ? accepted : rejected).push_back(part);
        }
        diagnostics.models[model].raw = computeRecordBounds(raw);
        diagnostics.models[model].main = mainBounds;
        diagnostics.models[model].acceptedParts = computeRecordBounds(accepted.size() > 1
            ? std::span<const assets::MeshRecord* const>(accepted.data() + 1, accepted.size() - 1)
            : std::span<const assets::MeshRecord* const>());
        diagnostics.models[model].rejectedParts = computeRecordBounds(rejected);
    }
    return diagnostics;
}

void dumpCarMeshDiagnostics(const CarMeshDiagnostics& diagnostics, std::ostream& out) {
    if (diagnostics.err != assets::CarModelError::Ok) {
        out << "car mesh diagnostics: conversion error=" << static_cast<int>(diagnostics.err) << '\n';
        return;
    }
    const auto printBounds = [&out](const CarMeshBounds& bounds) {
        out << bounds.vertexCount << " ["
            << bounds.min[0] << ',' << bounds.min[1] << ',' << bounds.min[2] << " .. "
            << bounds.max[0] << ',' << bounds.max[1] << ',' << bounds.max[2] << ']';
    };
    out << std::fixed << std::setprecision(3);
    for (std::size_t model = 0; model < diagnostics.models.size(); ++model) {
        const auto& row = diagnostics.models[model];
        out << "car model " << model << " raw verts/bbox=";
        printBounds(row.raw);
        out << " main=";
        printBounds(row.main);
        out << " accepted-parts=";
        printBounds(row.acceptedParts);
        out << " rejected-parts=";
        printBounds(row.rejectedParts);
        out << '\n';
    }
}

bool loadCarMeshFromFile(const std::string& path, CarMeshResult& out,
                          float targetLength, bool verbose) {
    std::ifstream f(path, std::ios::binary | std::ios::ate);
    if (!f) {
        out.err = assets::CarModelError::Truncated;
        return false;
    }
    const auto size = static_cast<std::size_t>(f.tellg());
    f.seekg(0);
    std::vector<std::uint8_t> bytes(size);
    if (size > 0) {
        f.read(reinterpret_cast<char*>(bytes.data()), static_cast<std::streamsize>(size));
        if (!f) {
            out.err = assets::CarModelError::Truncated;
            return false;
        }
    }
    if (verbose) {
        dumpCarMeshDiagnostics(inspectCarMeshDiagnostics(bytes, path), std::cout);
    }
    out = buildCarMeshFromCarFile(bytes, path, targetLength);
    return out.err == assets::CarModelError::Ok;
}

} // namespace rt::native::preview
