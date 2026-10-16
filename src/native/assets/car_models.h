#pragma once

// src/native/assets/car_models.h
//
// CAR car-model texture-binding completion: the pieces the mesh-only
// convertCarModels() deliberately leaves out (model_converter.h keeps image
// binding out of scope). Three stages over one CARn/Qnn.BIN file plus the
// ELF table that indexes them:
//
//   (a) Table E (ELF vaddr 0x29B438, 151 records tagged 0x00333388) ->
//       per-car disc-record info, and the Q-file 12-word header ->
//       {model starts, texture-chain start, file size}.
//   (b) Livery extraction: the file's own 2-upload texture chain at
//       [word 6, EOF) decoded by the existing image pipeline into one
//       RGBA8 ImageRecord (every disc car is self-textured).
//   (c) Binding: every material of every mesh resolved to that image and
//       the meshes lifted into fully-bound NativeModels via model_loader.
//
// Evidence (read-only; no guest code reused):
// - docs/native-reference/car-data-status.md: 12-word header grammar
//   (words 0..5 six model starts, [6] == texture-chain start == model-region
//   end, [7] == physical file size, [8..11] zero, 151/151 proven); texture
//   chain always a 0x4510 region holding exactly two uploads (PSMT8
//   128x128 -> DBP 0/DBW 2, then a PSMCT32 16x16 256-entry palette ->
//   DBP 64/DBW 1) terminated by an END tag, structurally identical across
//   all 151 cars while the livery pixels stay per-car unique; the loader's
//   0x34000 region gate ("CAR SIZE OVER! Q%02d:%X"); table-E record order ==
//   Q-number order 151/151 via the descriptor join (LBAs are NOT monotonic:
//   each 30-file folder is head-shuffled on disc, so no ordering is asserted).
// - docs/native-reference/asset-map.md: descriptor record =
//   {u32 tag 0x00333388, u32 sectorOffset, u32 sectorCount}, 12 bytes, read
//   at 0x29B438 + idx*12 by loadCarModel (0x20C7B8); sectorOffset is an
//   absolute disc LBA identical to the extract's .lbn_map.tsv value and
//   sectorCount == ceil(fileSize/2048); both PT_LOAD segments satisfy
//   ELF fileOffset = vaddr - 0x1FF000 (derived from the program headers).
// - The upload-chain decode itself is image_converter's frozen contract
//   (payload-linear, CSM1 swapBits34 palette read, byte0 = R); this module
//   only locates the chain and sequences the stages. The runtime DBP rebase
//   (+0x243E + slot*0x44 for request slots 15..22) is a VRAM-placement
//   concern the payload decode never needs, so it is not modelled here.
//
// Failure contract (mirrors model_converter.h): err != Ok means the value
// fields are default-empty; no partial, flagged output exists.

#include "container.h"
#include "model_converter.h"
#include "native_model.h"

#include <array>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace rt::native::assets {

// ── (a1) the CAR descriptor table (table E) ───────────────────────────────

// Q-number coverage Q00..Q150, no gaps (car-data-status.md inventory).
inline constexpr std::size_t kCarRecordCount = 151;

// Descriptor tag word shared by every record of every descriptor table.
inline constexpr std::uint32_t kCarDescriptorTag = 0x00333388u;

// One 12-byte table-E record: {tag, sectorOffset, sectorCount}. Only the
// two disc fields are carried; the tag is validated, not stored.
struct CarDescriptorRecord {
    std::uint32_t sectorOffset = 0;  // absolute disc LBA (.lbn_map.tsv join key)
    std::uint32_t sectorCount = 0;   // ceil(fileSize / 2048); read length in sectors
};

enum class CarTableError : std::uint8_t {
    Ok = 0,
    // The span cannot hold all kCarRecordCount records starting at
    // kCarTableElfFileOffset (not a valid table-E image).
    Truncated,
    // A record's word 0 != kCarDescriptorTag.
    BadTag,
};

struct CarTableResult {
    CarTableError err = CarTableError::Ok;
    std::size_t errorRecord = 0;  // index of the first bad-tag record
    // Record i describes car Q{i:02d}.BIN: table order == Q-number order
    // (PROVEN 151/151 by the descriptor join). Valid iff err == Ok.
    std::array<CarDescriptorRecord, kCarRecordCount> records{};
};

// Table E lives at ELF vaddr 0x29B438; both PT_LOAD segments give
// fileOffset = vaddr - 0x1FF000, so in the ELF *file image* (the contiguous
// bytes the disc extract holds, not a relocated load image) the table
// starts at 0x29B438 - 0x1FF000 = 0x9C438 and spans 151 * 12 bytes.
inline constexpr std::uint32_t kCarTableElfFileOffset = 0x29B438u - 0x1FF000u;
inline constexpr std::size_t kCarDescriptorRecordBytes = 12;

// Parses the whole table out of the SLUS_203.98 ELF file image. No ordering
// is imposed on the LBAs: the disc write order per folder is shuffled, only
// the record-index <-> Q-number correspondence is fixed.
CarTableResult parseCarTableE(std::span<const std::uint8_t> elfFile);

// ── (a2) the Q-file 12-word header ────────────────────────────────────────

inline constexpr std::size_t kCarHeaderWords = 12;   // 0x30 bytes
inline constexpr std::size_t kCarHeaderBytes = 0x30;
inline constexpr std::size_t kCarModelCount = 6;     // words 0..5

// The loader fatals above this bound on the model-region length word
// ("CAR SIZE OVER"; loadCarModel 0x20C7B8 step 2).
inline constexpr std::uint32_t kCarModelRegionMaxBytes = 0x34000u;

struct CarHeaderInfo {
    // Words 0..5: file-relative model starts, strictly spaced in every disc
    // file (model 0 starts at 0x30, i.e. right after this header).
    std::array<std::uint32_t, kCarModelCount> modelStarts{};
    // Word 6: end of the model region == start of the file's texture chain.
    std::uint32_t textureChainStart = 0;
    // Word 7: physical file size (equals the real byte count in 151/151
    // files; a truncated read is what breaks the equality).
    std::uint32_t fileSize = 0;
};

enum class CarHeaderError : std::uint8_t {
    Ok = 0,
    // Fewer than kCarHeaderBytes available.
    Truncated,
    // A header invariant failed; errorOffset locates the offending word (the
    // word's byte offset), or the texture-chain start for a bad chain tag.
    BadHeader,
};

struct CarHeaderResult {
    CarHeaderError err = CarHeaderError::Ok;
    std::uint64_t errorOffset = 0;
    CarHeaderInfo info{};  // valid iff err == Ok
};

// Parses and validates the 12-word header of one CARn/Qnn.BIN file:
//   - words 8..11 must be zero (the 151/151 corpus value),
//   - word 7 must equal the span's byte size,
//   - word 6 must not exceed the loader's 0x34000 region gate,
//   - word 0 must not overlap this 0x30-byte header,
//   - words 0..7 must be non-decreasing (monotonic model starts through the
//     texture-chain start and file size),
//   - when the file claims a texture chain (word 6 < word 7), its first DMA
//     tag must be CNT (tag byte 3 == 0x10), the loader's own chain-walk
//     check — anything else fatal there.
// An empty texture chain (word 6 == word 7) passes: it is not a header-
// level violation, it simply has no livery to extract.
CarHeaderResult parseCarHeader(std::span<const std::uint8_t> carFile);

// ── (b) per-car livery extraction ──────────────────────────────────────────

enum class CarLiveryError : std::uint8_t {
    Ok = 0,
    // parseCarHeader rejected the 12-word header.
    BadHeader,
    // The header was fine but the image pipeline rejected the texture chain
    // (malformed CNT/END walk, unparseable uploads, no decodable texture, or
    // the palette the chain names is not representable there).
    BadChain,
};

struct CarLiveryResult {
    CarLiveryError err = CarLiveryError::Ok;
    std::uint64_t errorOffset = 0;   // header word offset (BadHeader)
    std::uint32_t chainStart = 0;    // word 6 whenever the file holds it (diagnostics)
    ImageRecord image;              // valid iff err == Ok; the disc corpus yields
                                     // exactly one 128x128 R8G8B8A8 record per car
};

// Decodes one car's own texture chain — the region [word 6, EOF) — through
// convertImage() into the car's single livery texture record. provenance and
// variantId are opaque passthrough (they land verbatim in the record; the
// serialize/loadImage stages enforce their own validity). A header word-6
// chain of zero length (word 6 == word 7) has no livery and rejects with
// BadChain.
CarLiveryResult extractCarLivery(std::span<const std::uint8_t> carFile,
                                 const Provenance& provenance,
                                 const std::string& variantId);

// ── (c) material -> image binding ──────────────────────────────────────────

enum class CarBindError : std::uint8_t {
    Ok = 0,
    // No image records to bind to.
    NoImages,
    // imageIndex does not name an image record.
    BadImageIndex,
    // loadModel rejected a bound mesh (index/material inconsistency);
    // errorMesh locates the rejected mesh.
    LoaderError,
};

struct CarBindResult {
    CarBindError err = CarBindError::Ok;
    std::size_t errorMesh = 0;
    std::vector<NativeModel> models;  // one model per mesh; valid iff err == Ok
};

// Resolves EVERY material of EVERY mesh to images[imageIndex] (a Q-file
// carries exactly one livery texture shared by all of a car's models, so
// the natural binding index is 0) and loads each mesh into a NativeModel
// through loadModel(). The input meshes are copied, never modified: any
// previous material image value (including the convertCarModels sentinel
// 0xFFFFFFFF) is replaced inside the bind, so a successful result contains
// no unresolved sentinel anywhere.
CarBindResult bindCarModels(std::span<const MeshRecord> meshes,
                            std::span<const ImageRecord> images,
                            std::uint32_t imageIndex);

// ── one-file composite ─────────────────────────────────────────────────────

enum class CarAssetError : std::uint8_t {
    Ok = 0,
    // The 12-word header stage rejected the file (headerError carries why).
    BadHeader,
    // convertCarModels rejected the model region (modelError carries the
    // CarModelError, including MissingProvenance for an empty sourceFile or
    // an all-zero digest).
    ModelStage,
    // The livery stage rejected the texture chain (liveryError carries why).
    BadLivery,
    // The bind stage rejected a mesh (bindError carries why).
    BindStage,
};

struct CarAssetResult {
    CarAssetError err = CarAssetError::Ok;
    // Stage-detail codes, meaningful per the CarAssetError above.
    CarHeaderError headerError = CarHeaderError::Ok;
    CarModelError modelError = CarModelError::Ok;
    CarLiveryError liveryError = CarLiveryError::Ok;
    CarBindError bindError = CarBindError::Ok;
    // Header word byte offset (BadHeader), the failing chain offset
    // (ModelStage), the texture-chain start (BadLivery), or the rejected
    // mesh index (BindStage); 0 when not applicable.
    std::uint64_t errorOffset = 0;
    // Complete, wire-serializable container valid iff err == Ok: the car's
    // mesh records with every material resolved to images[0], plus the
    // livery image appended at index 0 (no unresolved sentinel anywhere).
    HostAssetContainer container;
    // One fully-bound NativeModel per container mesh, parallel to it.
    std::vector<NativeModel> models;
};

// Whole-car pipeline over one CARn/Qnn.BIN file: parseCarHeader ->
// convertCarModels -> extractCarLivery -> bindCarModels, in that order,
// failing fast on the first rejecting stage. The livery's provenance names
// the same caller-supplied blob identity as the meshes (sourceFile and
// sha256) with the chain's own span (offset word 6, length fileSize-word 6),
// and its variant id is "livery".
CarAssetResult loadCarAssets(std::span<const std::uint8_t> carFile,
                             const std::string& sourceFile,
                             const std::array<std::uint8_t, 32>& sha256);

} // namespace rt::native::assets
