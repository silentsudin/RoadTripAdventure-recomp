// src/native/assets/car_models.cpp
//
// Implementation of the CAR texture-binding completion stage (car_models.h).
// Reviewer notes:
// - Table E and the 12-word header are pure byte parsing over spans the
//   caller owns; nothing here does file or disc I/O (the caller reads its
//   own extract's ELF / CARn/Qnn.BIN bytes and hands them in).
// - Every read is bounds-checked before it happens: the file-size word
//   after a >= 0x30 check, every table word after the table-fit check, and
//   the chain's first tag after its own fit check. No offset is trusted
//   without a preceding range check.
// - The texture chain is located here and decoded entirely by the existing
//   image pipeline (image_converter.h); no CLUT/GS math is duplicated in
//   this file. Binding re-uses model_loader's loadModel unchanged.

#include "car_models.h"

#include "image_converter.h"
#include "model_loader.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <span>
#include <string>
#include <utility>
#include <vector>

namespace rt::native::assets {
namespace {

// The loader's chain-walk check: the first tag of a car texture chain is a
// CNT tag (byte 3 == 0x10); anything else is a fatal there and a BadHeader
// here (car-data-status.md, loadCarModel decompile).
constexpr std::uint8_t kDmaTagCntByte3 = 0x10u;
constexpr std::size_t kDmaTagBytes = 16;

std::uint32_t readU32LE(std::span<const std::uint8_t> b, std::size_t o) noexcept {
    return static_cast<std::uint32_t>(b[o]) |
           (static_cast<std::uint32_t>(b[o + 1]) << 8) |
           (static_cast<std::uint32_t>(b[o + 2]) << 16) |
           (static_cast<std::uint32_t>(b[o + 3]) << 24);
}

} // namespace

// ── (a1) table E ───────────────────────────────────────────────────────────

CarTableResult parseCarTableE(std::span<const std::uint8_t> elfFile) {
    CarTableResult result{};
    const std::uint64_t tableBytes =
        static_cast<std::uint64_t>(kCarRecordCount) * kCarDescriptorRecordBytes;
    const std::uint64_t tableEnd =
        static_cast<std::uint64_t>(kCarTableElfFileOffset) + tableBytes;
    if (tableEnd > elfFile.size()) {
        result.err = CarTableError::Truncated;
        return result;
    }

    std::array<CarDescriptorRecord, kCarRecordCount> records{};
    for (std::size_t i = 0; i < kCarRecordCount; ++i) {
        const std::size_t at =
            kCarTableElfFileOffset + i * kCarDescriptorRecordBytes;
        const std::uint32_t tag = readU32LE(elfFile, at);
        if (tag != kCarDescriptorTag) {
            result.err = CarTableError::BadTag;
            result.errorRecord = i;
            return result;  // records stay zeroed: value valid iff err == Ok
        }
        records[i].sectorOffset = readU32LE(elfFile, at + 4);
        records[i].sectorCount = readU32LE(elfFile, at + 8);
    }
    result.records = std::move(records);
    return result;
}

// ── (a2) the 12-word header ────────────────────────────────────────────────

CarHeaderResult parseCarHeader(std::span<const std::uint8_t> carFile) {
    CarHeaderResult result{};
    if (carFile.size() < kCarHeaderBytes) {
        result.err = CarHeaderError::Truncated;
        return result;
    }

    std::uint32_t word[kCarHeaderWords] = {};
    for (std::size_t i = 0; i < kCarHeaderWords; ++i) {
        word[i] = readU32LE(carFile, i * 4);
    }

    // Diagnostic best-effort: word 6 names the chain whenever it is readable.
    result.info.textureChainStart = word[6];

    // Words 8..11: zero in every disc file (corpus-proven); anything else
    // means this is not the 12-word header grammar.
    for (std::size_t i = kCarModelCount + 2; i < kCarHeaderWords; ++i) {
        if (word[i] != 0) {
            result.err = CarHeaderError::BadHeader;
            result.errorOffset = 4 * i;
            return result;
        }
    }
    // The size word must equal the real size; this is what catches a
    // truncated read of the file (151/151 on the disc corpus).
    if (static_cast<std::uint64_t>(word[7]) != carFile.size()) {
        result.err = CarHeaderError::BadHeader;
        result.errorOffset = 4 * 7;
        return result;
    }
    // The loader's own gate on the model-region length ("CAR SIZE OVER").
    if (word[6] > kCarModelRegionMaxBytes) {
        result.err = CarHeaderError::BadHeader;
        result.errorOffset = 4 * 6;
        return result;
    }
    // No model region may overlap the 0x30-byte header (word 0 is 0x30 in
    // every disc file: model 0 starts directly after it).
    if (word[0] < kCarHeaderBytes) {
        result.err = CarHeaderError::BadHeader;
        result.errorOffset = 0;
        return result;
    }
    // Monotonic non-decreasing words 0..7: model starts ascending through
    // the texture-chain start, and the chain start within the file size
    // (hence within the span, given the size-word equality above). Equal
    // adjacent model starts fall out here only if they also break
    // monotonicity; an empty region is convertCarModels' Truncated, not a
    // header-level claim, and is left to it.
    for (std::size_t i = 0; i + 1 < kCarModelCount + 2; ++i) {
        if (word[i] > word[i + 1]) {
            result.err = CarHeaderError::BadHeader;
            result.errorOffset = 4 * (i + 1);
            return result;
        }
    }
    // When the file claims a texture chain, its first DMA tag must be CNT —
    // the check the loader's own chain walk applies before consuming it
    // (model_converter enforces the identical claim; repeated here so a
    // header-only parse never reports a chain it cannot walk).
    if (static_cast<std::uint64_t>(word[6]) < static_cast<std::uint64_t>(word[7])) {
        const std::uint64_t chainStart = word[6];
        if (chainStart + kDmaTagBytes > carFile.size() ||
            carFile[static_cast<std::size_t>(chainStart) + 3] != kDmaTagCntByte3) {
            result.err = CarHeaderError::BadHeader;
            result.errorOffset = chainStart;
            return result;
        }
    }

    for (std::size_t i = 0; i < kCarModelCount; ++i) {
        result.info.modelStarts[i] = word[i];
    }
    result.info.textureChainStart = word[6];
    result.info.fileSize = word[7];
    return result;
}

// ── (b) livery extraction ──────────────────────────────────────────────────

CarLiveryResult extractCarLivery(std::span<const std::uint8_t> carFile,
                                  const Provenance& provenance,
                                  const std::string& variantId) {
    CarLiveryResult result{};

    const CarHeaderResult header = parseCarHeader(carFile);
    if (header.err != CarHeaderError::Ok) {
        result.err = CarLiveryError::BadHeader;
        result.errorOffset = header.errorOffset;
        result.chainStart = header.info.textureChainStart;
        return result;
    }
    result.chainStart = header.info.textureChainStart;

    // [word 6, EOF) is the file's whole texture chain. The chain's own END
    // tag terminates the walk inside the image pipeline, so trailing bytes
    // of the region after that tag (0x20 tool-junk bytes on every disc
    // file) are a bounds limit only, never consumed data. A zero-length
    // chain (word 6 == word 7) rejects in the pipeline below.
    const std::size_t start = static_cast<std::size_t>(result.chainStart);
    const std::span<const std::uint8_t> chain = carFile.subspan(start);

    ImageRecord image = convertImage(chain, provenance, variantId);
    if (image.width == 0) {
        // The pipeline's malformed/truncated sentinel is a default record.
        result.err = CarLiveryError::BadChain;
        return result;
    }
    result.image = std::move(image);
    return result;
}

// ── (c) binding ────────────────────────────────────────────────────────────

CarBindResult bindCarModels(std::span<const MeshRecord> meshes,
                            std::span<const ImageRecord> images,
                            std::uint32_t imageIndex) {
    CarBindResult result{};
    if (images.empty()) {
        result.err = CarBindError::NoImages;
        return result;
    }
    if (static_cast<std::uint64_t>(imageIndex) >=
        static_cast<std::uint64_t>(images.size())) {
        result.err = CarBindError::BadImageIndex;
        return result;
    }

    std::vector<NativeModel> models;
    models.reserve(meshes.size());
    for (std::size_t i = 0; i < meshes.size(); ++i) {
        // A bound copy: the originals never gain a side effect here, and a
        // stale image value (the 0xFFFFFFFF sentinel included) cannot leak
        // into the result.
        MeshRecord bound = meshes[i];
        for (MaterialRef& material : bound.materials) {
            material.imageIndex = imageIndex;
        }
        try {
            models.push_back(loadModel(bound, images));
        } catch (const std::exception&) {
            result.err = CarBindError::LoaderError;
            result.errorMesh = i;
            return result;  // models stay empty: value valid iff err == Ok
        }
    }
    result.models = std::move(models);
    return result;
}

// ── composite ──────────────────────────────────────────────────────────────

CarAssetResult loadCarAssets(std::span<const std::uint8_t> carFile,
                             const std::string& sourceFile,
                             const std::array<std::uint8_t, 32>& sha256) {
    CarAssetResult result{};

    // Stage 1 — header (locate the models and the chain up front).
    const CarHeaderResult header = parseCarHeader(carFile);
    if (header.err != CarHeaderError::Ok) {
        result.err = CarAssetError::BadHeader;
        result.headerError = header.err;
        result.errorOffset = header.errorOffset;
        return result;
    }

    // Stage 2 — the six models through the mesh-only converter. Its 151-file
    // proven header/blog checks overlap this file's 12-word view by design;
    // this stage still owns provenance completeness and chain decoding.
    const CarModelResult meshes = convertCarModels(carFile, sourceFile, sha256);
    if (meshes.err != CarModelError::Ok) {
        result.err = CarAssetError::ModelStage;
        result.modelError = meshes.err;
        result.errorOffset = meshes.errorOffset;
        return result;
    }

    // Stage 3 — the livery.
    Provenance liveryProvenance{};
    liveryProvenance.sourceFile = sourceFile;
    liveryProvenance.offset = header.info.textureChainStart;
    liveryProvenance.length =
        carFile.size() - static_cast<std::size_t>(header.info.textureChainStart);
    liveryProvenance.sha256 = sha256;
    const CarLiveryResult livery =
        extractCarLivery(carFile, liveryProvenance, "livery");
    if (livery.err != CarLiveryError::Ok) {
        result.err = CarAssetError::BadLivery;
        result.liveryError = livery.err;
        result.errorOffset = livery.chainStart;
        return result;
    }

    // Stage 4 — bind inside the container and lift to NativeModels. One
    // livery per file means one image at index 0 for every material.
    HostAssetContainer container = std::move(meshes.value);
    for (MeshRecord& mesh : container.meshes) {
        for (MaterialRef& material : mesh.materials) {
            material.imageIndex = 0;
        }
    }
    container.images.push_back(std::move(livery.image));

    const CarBindResult bound = bindCarModels(container.meshes, container.images, 0);
    if (bound.err != CarBindError::Ok) {
        result.err = CarAssetError::BindStage;
        result.bindError = bound.err;
        result.errorOffset = bound.errorMesh;
        return result;  // container stays default: value valid iff err == Ok
    }

    result.container = std::move(container);
    result.models = std::move(bound.models);
    return result;
}

} // namespace rt::native::assets
