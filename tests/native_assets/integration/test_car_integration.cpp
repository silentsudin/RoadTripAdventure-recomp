// tests/native_assets/integration/test_car_integration.cpp
//
// End-to-end integration test for the CAR model side of the native asset
// pipeline, exercised against the user's own disc file CAR0/Q00.BIN at run
// time: read -> convertCarModels -> serialize -> deserialize -> loadModel.
// No game data ships with this test; the disc root comes from the
// environment (RT_DISC, default "~/Library/Application Support/
// RoadTripRecomp/disc"). A missing file is a loud setup failure (nonzero
// exit), not a silent skip: this suite exists to run the real pipeline over
// real bytes.
//
// Full-pipeline contract asserted on the real file:
//   1. The CAR header invariants convertCarModels enforces hold for the real
//      disc file (size word, monotonic model starts, the loader's 0x34000
//      region bound, texture chain starting with a CNT tag).
//   2. convertCarModels returns Ok with real meshes: at least six mesh
//      records (one per model main chain, plus count-gated part chains),
//      every mesh with vertices, indices and materials; every material range
//      non-empty, tiling the index buffer exactly; every MaterialRef keeps
//      the unresolved image sentinel 0xFFFFFFFF (image binding is a separate
//      pipeline step); every index in range; every vertex float finite.
//   3. Every mesh provenance points back into the source file: the real
//      source path, the caller's digest, and a nonzero 16-aligned span inside
//      the model region; mesh 0's span starts at model 0 + 0x10 (its main
//      chain, per the header-driven locating rule).
//   4. serialize -> deserialize round-trips to an equal container, and
//      re-serializing is byte-stable.
//   5. loadModel yields a NativeModel consistent with each mesh (same
//      vertices and indices, one unresolved material image slot per
//      material), both before and after the wire round trip.
//   6. car_models completion over the same real file + the real ELF:
//      parseCarTableE returns the 151 tagged descriptor records (record 0
//      joins this Q00 file by its exact sector count); parseCarHeader
//      exposes the 12-word header (words 8..11 zero, chain start + 0x4510 ==
//      EOF); extractCarLivery decodes the file's own PSMT8 128x128 livery
//      (single palette bank); bindCarModels resolves every material to that
//      image; loadCarAssets composes all stages into a fully-bound
//      container (no unresolved sentinel, serialize round-trip intact).
//
// Failure accounting is plain counters and if-statements (no assert()), so
// behaviour is NDEBUG-independent; any failed check exits nonzero.

#include "../../../src/native/assets/car_models.h"
#include "../../../src/native/assets/container.h"
#include "../../../src/native/assets/image_converter.h"
#include "../../../src/native/assets/model_converter.h"
#include "../../../src/native/assets/model_loader.h"

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <span>
#include <string>
#include <vector>

using rt::native::assets::CarAssetError;
using rt::native::assets::CarAssetResult;
using rt::native::assets::CarBindError;
using rt::native::assets::CarBindResult;
using rt::native::assets::CarHeaderError;
using rt::native::assets::CarHeaderResult;
using rt::native::assets::CarLiveryError;
using rt::native::assets::CarLiveryResult;
using rt::native::assets::CarModelError;
using rt::native::assets::CarModelResult;
using rt::native::assets::CarTableError;
using rt::native::assets::CarTableResult;
using rt::native::assets::ContainerError;
using rt::native::assets::ContainerResult;
using rt::native::assets::HostAssetContainer;
using rt::native::assets::ImageRecord;
using rt::native::assets::MaterialRef;
using rt::native::assets::MeshRecord;
using rt::native::assets::NativeModel;
using rt::native::assets::Provenance;
using rt::native::assets::Vertex;
using rt::native::assets::bindCarModels;
using rt::native::assets::convertCarModels;
using rt::native::assets::convertImageBankCount;
using rt::native::assets::deserialize;
using rt::native::assets::extractCarLivery;
using rt::native::assets::hasValidProvenance;
using rt::native::assets::kContainerVersion;
using rt::native::assets::kCarRecordCount;
using rt::native::assets::loadCarAssets;
using rt::native::assets::loadModel;
using rt::native::assets::parseCarHeader;
using rt::native::assets::parseCarTableE;
using rt::native::assets::serialize;

namespace {

// ---------------------------------------------------------------------------
// Failure accounting (NDEBUG-independent; no assert() anywhere in this file).
// ---------------------------------------------------------------------------
int g_checks = 0;
int g_failures = 0;

void check(bool cond, const std::string& what) {
    ++g_checks;
    if (!cond) {
        ++g_failures;
        std::cerr << "FAIL: " << what << '\n';
    }
}

// Prints the summary line and returns the process exit code.
int finish() {
    if (g_failures > 0) {
        std::cerr << "test_car_integration: " << g_failures << " of " << g_checks
                  << " checks FAILED\n";
        return EXIT_FAILURE;
    }
    std::cout << "test_car_integration: " << g_checks << " checks passed\n";
    return EXIT_SUCCESS;
}

using Bytes = std::vector<std::uint8_t>;
using Digest = std::array<std::uint8_t, 32>;

std::uint32_t readU32LE(const Bytes& b, std::size_t o) {
    return static_cast<std::uint32_t>(b[o]) |
           (static_cast<std::uint32_t>(b[o + 1]) << 8) |
           (static_cast<std::uint32_t>(b[o + 2]) << 16) |
           (static_cast<std::uint32_t>(b[o + 3]) << 24);
}

// Placeholder provenance digest: FNV-1a over the bytes, expanded to 32 bytes
// by a xorshift64 stream seeded non-zero. NOT SHA-256: hasValidProvenance()
// only rejects the all-zero digest (same placeholder the pipeline tools use;
// no game bytes are embedded here).
Digest placeholderDigest(std::span<const std::uint8_t> bytes) {
    std::uint64_t hash = 0xcbf29ce484222325ull; // FNV-1a 64-bit offset basis
    for (std::uint8_t byte : bytes) {
        hash ^= byte;
        hash *= 0x100000001b3ull; // FNV-1a 64-bit prime
    }
    Digest digest{};
    std::uint64_t state = hash | 1ull; // never zero, so the digest is never all-zero
    for (std::size_t i = 0; i < digest.size(); ++i) {
        state ^= state << 13;
        state ^= state >> 7;
        state ^= state << 17; // xorshift64
        digest[i] = static_cast<std::uint8_t>((state >> 24) & 0xFFu);
    }
    return digest;
}

// Disc root from the environment: RT_DISC when set, otherwise the standard
// extracted-disc location under the user's home directory.
std::string discRoot() {
    if (const char* env = std::getenv("RT_DISC"); env != nullptr && env[0] != '\0') {
        return env;
    }
    const char* home = std::getenv("HOME");
    if (home == nullptr || home[0] == '\0') {
        return {};
    }
    return (std::filesystem::path(home) / "Library/Application Support/RoadTripRecomp/disc")
        .string();
}

bool readFileBytes(const std::string& path, Bytes& out) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        return false;
    }
    out.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    return !file.bad();
}

bool vertexFinite(const Vertex& v) {
    const float fs[8] = {v.px, v.py, v.pz, v.nx, v.ny, v.nz, v.u, v.v};
    for (float f : fs) {
        if (!std::isfinite(f)) {
            return false;
        }
    }
    return true;
}

// Checks the header invariants convertCarModels enforces, so a conversion
// failure on real data is distinguishable from a header surprise. Returns
// false when the file is too short to hold the header (nothing else can run).
bool checkHeader(const Bytes& bytes, std::uint32_t (&word)[8]) {
    check(bytes.size() >= 0x20, "car file holds the eight header words");
    if (bytes.size() < 0x20) {
        return false;
    }
    for (std::size_t i = 0; i < 8; ++i) {
        word[i] = readU32LE(bytes, i * 4);
    }
    check(static_cast<std::uint64_t>(word[7]) == bytes.size(),
          "header word +0x1C equals the real file size");
    check(word[6] <= 0x34000u, "header word +0x18 within the loader's 0x34000 region bound");
    check(word[0] >= 0x20u, "model 0 starts at or after the header");
    for (std::size_t i = 0; i + 1 < 8; ++i) {
        check(word[i] <= word[i + 1],
              "header word " + std::to_string(i) + " <= word " + std::to_string(i + 1) +
                  " (monotonic starts)");
    }
    const std::size_t chainStart = static_cast<std::size_t>(word[6]);
    check(chainStart + 16 <= bytes.size() && bytes[chainStart + 3] == 0x10u,
          "texture chain at word +0x18 starts with a CNT tag");
    return true;
}

// Per-mesh geometry/material/provenance assertions on the real conversion.
void checkMesh(const MeshRecord& mesh, std::size_t i, const std::string& path,
               const Digest& digest, std::uint32_t regionStart, std::uint32_t regionEnd) {
    const std::string tag = "mesh " + std::to_string(i) + ": ";
    check(!mesh.vertices.empty(), tag + "vertices non-empty (real geometry)");
    check(!mesh.indices.empty(), tag + "indices non-empty");
    check(!mesh.materials.empty(), tag + "materials non-empty");

    bool boundsOk = true;
    for (std::uint32_t idx : mesh.indices) {
        if (static_cast<std::uint64_t>(idx) >= mesh.vertices.size()) {
            boundsOk = false;
        }
    }
    check(boundsOk, tag + "every index addresses a vertex of this mesh");

    bool finite = true;
    for (const Vertex& v : mesh.vertices) {
        if (!vertexFinite(v)) {
            finite = false;
        }
    }
    check(finite, tag + "every vertex float finite (serialize() would accept it)");

    // Materials tile the index buffer exactly: first range starts at 0, each
    // range continues the previous one, and the last ends at indices.size().
    bool tiling = !mesh.materials.empty();
    bool unresolved = true;
    bool nonEmptyRanges = true;
    std::uint64_t run = 0;
    for (const MaterialRef& mat : mesh.materials) {
        if (mat.firstIndex != run) {
            tiling = false;
        }
        run += mat.indexCount;
        if (mat.indexCount == 0) {
            nonEmptyRanges = false;
        }
        if (mat.imageIndex != 0xFFFFFFFFu) {
            unresolved = false;
        }
    }
    if (run != mesh.indices.size()) {
        tiling = false;
    }
    check(tiling, tag + "material ranges tile the index buffer exactly");
    check(nonEmptyRanges, tag + "every material range is non-empty");
    check(unresolved, tag + "every material keeps the unresolved imageIndex 0xFFFFFFFF");

    check(mesh.provenance.sourceFile == path, tag + "provenance names the real source file");
    check(mesh.provenance.sha256 == digest, tag + "provenance carries the caller's digest");
    check(hasValidProvenance(mesh.provenance), tag + "provenance is valid");
    check(mesh.provenance.length > 0, tag + "provenance span is nonzero");
    check(mesh.provenance.offset % 16 == 0, tag + "provenance offset is 16-aligned (a DMA tag)");
    check(mesh.provenance.offset >= regionStart &&
              mesh.provenance.offset + mesh.provenance.length <= regionEnd,
          tag + "provenance span lies inside the model region");
}

// loadModel consistency for one mesh, before/after the wire round trip.
void checkLoadModel(const MeshRecord& mesh, const MeshRecord* roundTripped, std::size_t i) {
    const std::string tag = "mesh " + std::to_string(i) + ": ";
    try {
        const NativeModel model = loadModel(mesh, {});
        check(model.vertices == mesh.vertices, tag + "loadModel copies the vertices verbatim");
        check(model.indices == mesh.indices, tag + "loadModel copies the indices verbatim");
        check(model.materialImage.size() == mesh.materials.size(),
              tag + "loadModel yields one material image slot per material");
        bool allUnresolved = true;
        for (std::uint32_t imageIndex : model.materialImage) {
            if (imageIndex != 0xFFFFFFFFu) {
                allUnresolved = false;
            }
        }
        check(allUnresolved, tag + "loadModel keeps every material image unresolved");

        if (roundTripped != nullptr) {
            const NativeModel model2 = loadModel(*roundTripped, {});
            check(model2.vertices == model.vertices && model2.indices == model.indices &&
                      model2.materialImage == model.materialImage,
                  tag + "loadModel agrees across the serialize/deserialize round trip");
        }
    } catch (const std::exception& e) {
        check(false, tag + "loadModel threw: " + e.what());
    }
}

} // namespace

int main() {
    const std::string root = discRoot();
    if (root.empty()) {
        std::cerr << "test_car_integration: cannot resolve the disc root "
                     "(set RT_DISC; HOME is unset)\n";
        return EXIT_FAILURE;
    }
    const std::filesystem::path carPath = std::filesystem::path(root) / "CAR0" / "Q00.BIN";
    Bytes bytes;
    if (!readFileBytes(carPath.string(), bytes)) {
        std::cerr << "test_car_integration: cannot read '" << carPath.string()
                  << "'; set RT_DISC to the extracted disc root "
                     "(no game data ships with this test)\n";
        return EXIT_FAILURE;
    }
    std::cout << "test_car_integration: " << carPath.string() << " (" << bytes.size()
              << " bytes)\n";

    // --- 1. header invariants on the real file ------------------------------
    std::uint32_t word[8] = {};
    if (!checkHeader(bytes, word)) {
        return finish();
    }

    // --- 2./3. full conversion of the real models ---------------------------
    const Digest digest = placeholderDigest(bytes);
    const CarModelResult result = convertCarModels(bytes, carPath.string(), digest);
    check(result.err == CarModelError::Ok, "convertCarModels accepts the real disc file");
    if (result.err != CarModelError::Ok) {
        std::cerr << "test_car_integration: conversion failed with code "
                  << static_cast<int>(result.err) << " at offset 0x" << std::hex
                  << result.errorOffset << std::dec << '\n';
        return finish();
    }
    check(result.errorOffset == 0, "successful conversion carries no error offset");

    const HostAssetContainer& container = result.value;
    check(container.version == kContainerVersion, "container version is kContainerVersion");
    check(container.images.empty(),
          "mesh-only container: image binding is a separate pipeline step");
    check(container.meshes.size() >= 6, "at least six real meshes (one per model minimum)");

    std::uint64_t totalVertices = 0;
    std::uint64_t totalIndices = 0;
    for (std::size_t i = 0; i < container.meshes.size(); ++i) {
        const MeshRecord& mesh = container.meshes[i];
        checkMesh(mesh, i, carPath.string(), digest, word[0], word[6]);
        totalVertices += mesh.vertices.size();
        totalIndices += mesh.indices.size();
    }
    check(totalVertices > 0 && totalIndices > 0, "the car yields real geometry overall");
    if (!container.meshes.empty()) {
        check(container.meshes[0].provenance.offset == static_cast<std::uint64_t>(word[0]) + 0x10,
              "mesh 0 starts at model 0 + 0x10 (its main chain)");
    }

    // --- 4. serialize -> deserialize -> byte-stable re-serialize ------------
    Bytes wire;
    bool serialized = true;
    try {
        wire = serialize(container);
    } catch (const std::exception& e) {
        serialized = false;
        check(false, std::string("serialize threw: ") + e.what());
    }
    if (serialized) {
        check(wire.size() >= 16 && wire[0] == 'R' && wire[1] == 'T' && wire[2] == 'A' &&
                  wire[3] == 'C',
              "serialized container starts with the RTAC magic");
        const ContainerResult back = deserialize(wire);
        check(back.err == ContainerError::Ok, "deserialize accepts its own wire format");
        check(back.value == container, "deserialize round-trips to an equal container");
        try {
            check(serialize(back.value) == wire,
                  "re-serializing the round-tripped container is byte-stable");
        } catch (const std::exception& e) {
            check(false, std::string("re-serialize threw: ") + e.what());
        }

        // --- 5. loadModel consistency, before and after the round trip ------
        for (std::size_t i = 0; i < container.meshes.size(); ++i) {
            const MeshRecord* roundTripped =
                (back.err == ContainerError::Ok &&
                 back.value.meshes.size() == container.meshes.size())
                    ? &back.value.meshes[i]
                    : nullptr;
            checkLoadModel(container.meshes[i], roundTripped, i);
        }
    }

    // --- 6. car_models: table E, 12-word header, livery, bound models -----
    {
        const std::filesystem::path elfPath =
            std::filesystem::path(root) / "SLUS_203.98";
        Bytes elf;
        check(readFileBytes(elfPath.string(), elf), "the disc ELF is readable");
        if (elf.empty()) {
            std::cerr << "test_car_integration: cannot read '" << elfPath.string()
                      << "'; table-E/livery checks skipped\n";
        }

        if (!elf.empty()) {
            // Table E out of the real ELF: 151 tagged records, record 0
            // joining this Q00 file by its exact (LBA, sector count).
            const CarTableResult table = parseCarTableE(elf);
            check(table.err == CarTableError::Ok,
                  "table E parses 151 tagged records out of the real ELF");
            check(table.records.size() == kCarRecordCount,
                  "table E holds one record per Q-file (151)");
            if (table.err == CarTableError::Ok) {
                check(table.records.front().sectorCount == (bytes.size() + 2047) / 2048,
                      "table-E record 0 (Q00) names this file's exact sector count");
                check(table.records.front().sectorOffset > 0,
                      "table-E record 0 carries a real disc LBA");
            }

            // The 12-word header over the same real file.
            const CarHeaderResult header = parseCarHeader(bytes);
            check(header.err == CarHeaderError::Ok,
                  "parseCarHeader accepts the real Q-file header");
            if (header.err == CarHeaderError::Ok) {
                check(header.info.fileSize == bytes.size(),
                      "header info: file size word equals the real size");
                check(header.info.textureChainStart == word[6],
                      "header info: texture-chain start equals word +0x18");
                check(header.info.modelStarts[0] == word[0],
                      "header info: model starts come from words 0..5");
                check(header.info.textureChainStart + 0x4510 == bytes.size(),
                      "header info: the chain region is the trailing 0x4510 bytes");
                bool paddingZero = true;
                for (std::size_t o = 0x20; o < 0x30; o += 4) {
                    if (readU32LE(bytes, o) != 0) {
                        paddingZero = false;
                    }
                }
                check(paddingZero, "header words 8..11 (0x20..0x2F) are zero");
            }

            // Livery: the file's own PSMT8 128x128 texture decodes through
            // the image pipeline with the single palette in the same chain.
            Provenance liveryProvenance{};
            liveryProvenance.sourceFile = carPath.string();
            liveryProvenance.offset = word[6];
            liveryProvenance.length = bytes.size() - static_cast<std::size_t>(word[6]);
            liveryProvenance.sha256 = digest;
            const CarLiveryResult livery = extractCarLivery(bytes, liveryProvenance, "livery");
            check(livery.err == CarLiveryError::Ok,
                  "extractCarLivery decodes the real car's own texture chain");
            check(livery.chainStart == word[6],
                  "livery result names the real chain start");
            if (livery.err == CarLiveryError::Ok) {
                check(livery.image.width == 128 && livery.image.height == 128,
                      "real livery is the pinned PSMT8 128x128 texture");
                check(livery.image.mipCount == 1,
                      "real livery carries a single mip level");
                check(livery.image.pixels.size() == 128u * 128u * 4u,
                      "real livery pixels fill 128*128*4 bytes");
                check(livery.image.provenance == liveryProvenance,
                      "real livery provenance is the chain span identity");
                check(livery.image.sourceVariantId == "livery",
                      "real livery variant id propagated");
            }
            check(convertImageBankCount(
                      std::span<const std::uint8_t>(bytes).subspan(word[6])) == 1,
                  "the real chain offers exactly one palette selection");

            // Binding: every mesh material resolves to the livery image.
            if (livery.err == CarLiveryError::Ok) {
                const ImageRecord liveryImages[1] = {livery.image};
                const CarBindResult bound = bindCarModels(
                    result.value.meshes, std::span<const ImageRecord>(liveryImages, 1), 0);
                check(bound.err == CarBindError::Ok,
                      "bindCarModels resolves the real car's whole mesh set");
                check(bound.models.size() == result.value.meshes.size(),
                      "binding yields one NativeModel per real mesh");
                if (bound.err == CarBindError::Ok) {
                    bool allZero = true;
                    for (const NativeModel& model : bound.models) {
                        for (std::uint32_t imageIndex : model.materialImage) {
                            if (imageIndex != 0) {
                                allZero = false;
                            }
                        }
                    }
                    check(allZero,
                          "every material of every bound model resolves to image 0 "
                          "(no sentinel left)");
                }
            }

            // The composite over the same real bytes: fully-bound, wire-
            // serializable container plus its NativeModels.
            const CarAssetResult assets = loadCarAssets(bytes, carPath.string(), digest);
            check(assets.err == CarAssetError::Ok,
                  "loadCarAssets loads the real car end to end");
            if (assets.err == CarAssetError::Ok) {
                check(assets.container.meshes.size() == result.value.meshes.size(),
                      "composite container keeps every mesh convertCarModels made");
                check(assets.container.images.size() == 1,
                      "composite container carries exactly the one livery image");
                check(assets.container.version == kContainerVersion,
                      "composite container version is kContainerVersion");
                bool noSentinel = true;
                bool allResolved = true;
                for (const MeshRecord& mesh : assets.container.meshes) {
                    for (const MaterialRef& material : mesh.materials) {
                        if (material.imageIndex == 0xFFFFFFFFu) {
                            noSentinel = false;
                        }
                        if (material.imageIndex >= assets.container.images.size()) {
                            allResolved = false;
                        }
                    }
                }
                check(noSentinel, "composite container keeps no unresolved sentinel");
                check(allResolved, "composite materials all point at the livery image");
                check(assets.models.size() == assets.container.meshes.size(),
                      "composite models parallel the container meshes");

                try {
                    const Bytes boundWire = serialize(assets.container);
                    const ContainerResult boundBack = deserialize(boundWire);
                    check(boundBack.err == ContainerError::Ok,
                          "the fully-bound container serializes and deserializes");
                    check(boundBack.value == assets.container,
                          "the bound container round-trips to an equal value");
                } catch (const std::exception& e) {
                    check(false, std::string("bound serialize threw: ") + e.what());
                }
            }
        }
    }

    return finish();
}
