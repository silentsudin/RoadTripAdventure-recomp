// tests/native_assets/test_car_models.cpp
//
// Synthetic contract tests for the CAR texture-binding completion stage
// (src/native/assets/car_models.{h,cpp}). Every input byte is hand-built
// here to reproduce the documented grammar — no game bytes, no disc reads:
//
//   - a synthetic ELF file image with 151 table-E descriptor records
//     {tag 0x00333388, sectorOffset, sectorCount} at file offset
//     0x29B438 - 0x1FF000 (= 0x9C438),
//   - synthetic Q-files with the 12-word header (six model starts, texture-
//     chain start, file size, four zero words),
//   - a disc-shaped livery texture chain: CNT(qwc 1030) PSMT8 128x128
//     upload at DBP 0/DBW 2, CNT(qwc 70) PSMCT32 16x16 256-entry palette at
//     DBP 64/DBW 1, END tag, and 0x20 trailing bytes inside the 0x4510
//     region — with tool-junk values placed exactly in the bit fields the
//     build tool stores its pointers into (the fields the GIF masks away),
//     to pin that they are ignored,
//   - synthetic mesh records and image records for the binding stage.
//
// Expected pixel values are derived from the decode math the image pipeline
// documents (CSM1 swapBits34 palette read, byte0 = R, TEXA opaque alpha
// 0x80 exported as min(255, a * 255 / 128), all-zero entry exporting 0).
//
// Failure accounting is plain counters and if-statements (no assert()), so
// behaviour is NDEBUG-independent; any failed check exits nonzero.

#include "../../src/native/assets/car_models.h"
#include "../../src/native/assets/container.h"
#include "../../src/native/assets/image_converter.h"

#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
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
using rt::native::assets::CarTableError;
using rt::native::assets::CarTableResult;
using rt::native::assets::HostAssetContainer;
using rt::native::assets::ImageRecord;
using rt::native::assets::MaterialRef;
using rt::native::assets::MeshRecord;
using rt::native::assets::NativeModel;
using rt::native::assets::PixelFormat;
using rt::native::assets::Provenance;
using rt::native::assets::bindCarModels;
using rt::native::assets::convertImageBankCount;
using rt::native::assets::deserialize;
using rt::native::assets::extractCarLivery;
using rt::native::assets::kCarDescriptorRecordBytes;
using rt::native::assets::kCarDescriptorTag;
using rt::native::assets::kCarHeaderBytes;
using rt::native::assets::kCarModelCount;
using rt::native::assets::kCarModelRegionMaxBytes;
using rt::native::assets::kCarRecordCount;
using rt::native::assets::kCarTableElfFileOffset;
using rt::native::assets::kContainerVersion;
using rt::native::assets::loadCarAssets;
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

using Bytes = std::vector<std::uint8_t>;
using Digest = std::array<std::uint8_t, 32>;

const std::string kSourceFile = "synthetic.car";

Digest testDigest() {
    Digest d{};
    d.fill(0xAB);
    return d;
}

// ---------------------------------------------------------------------------
// Little-endian writers for authoring fixture bytes by hand.
// ---------------------------------------------------------------------------
void putU16(Bytes& b, std::uint16_t v) {
    b.push_back(static_cast<std::uint8_t>(v & 0xFFu));
    b.push_back(static_cast<std::uint8_t>((v >> 8) & 0xFFu));
}

void putU32(Bytes& b, std::uint32_t v) {
    for (int i = 0; i < 4; ++i) {
        b.push_back(static_cast<std::uint8_t>(v >> (8 * i)));
    }
}

void putU32At(Bytes& b, std::size_t pos, std::uint32_t v) {
    for (int i = 0; i < 4; ++i) {
        b[pos + static_cast<std::size_t>(i)] = static_cast<std::uint8_t>(v >> (8 * i));
    }
}

void putU64(Bytes& b, std::uint64_t v) {
    for (int i = 0; i < 8; ++i) {
        b.push_back(static_cast<std::uint8_t>(v >> (8 * i)));
    }
}

void putFloat(Bytes& b, float f) {
    putU32(b, std::bit_cast<std::uint32_t>(f));
}

void setU32(Bytes& b, std::size_t off, std::uint32_t v) {
    putU32At(b, off, v);
}

std::uint32_t getU32(const Bytes& b, std::size_t off) {
    return static_cast<std::uint32_t>(b[off]) |
           (static_cast<std::uint32_t>(b[off + 1]) << 8) |
           (static_cast<std::uint32_t>(b[off + 2]) << 16) |
           (static_cast<std::uint32_t>(b[off + 3]) << 24);
}

// ---------------------------------------------------------------------------
// Table-E fixture: an ELF file image holding 151 descriptor records.
// ---------------------------------------------------------------------------
std::uint32_t fixtureLba(std::size_t recordIndex) {
    return static_cast<std::uint32_t>(0x4D84u + recordIndex * 7u);
}

std::uint32_t fixtureSectors(std::size_t recordIndex) {
    return static_cast<std::uint32_t>(80u + recordIndex % 13u);
}

std::size_t elfFixtureSize() {
    return static_cast<std::size_t>(kCarTableElfFileOffset) +
           kCarRecordCount * kCarDescriptorRecordBytes;
}

Bytes makeElf() {
    Bytes elf(elfFixtureSize(), 0x00);
    for (std::size_t i = 0; i < kCarRecordCount; ++i) {
        const std::size_t at =
            static_cast<std::size_t>(kCarTableElfFileOffset) + i * kCarDescriptorRecordBytes;
        setU32(elf, at + 0, kCarDescriptorTag);
        setU32(elf, at + 4, fixtureLba(i));
        setU32(elf, at + 8, fixtureSectors(i));
    }
    return elf;
}

// ---------------------------------------------------------------------------
// VIF/DMA chain fixtures for the model region (same hand-built wire shapes
// test_model_converter authors: CNT tag carrying the first two VIF codes in
// its upper qword, END tag completing the chain).
// ---------------------------------------------------------------------------
Bytes dmaChain(Bytes stream) {
    std::size_t q = 0;
    if (stream.size() > 16) {
        q = (stream.size() - 16 + 15) / 16;
    }
    stream.resize(16 + 16 * q, 0);

    Bytes chain;
    putU64(chain, static_cast<std::uint64_t>(q) | (1ull << 28)); // CNT, qwc = q
    chain.insert(chain.end(), stream.begin(), stream.begin() + 8);
    chain.insert(chain.end(), stream.begin() + 8,
                 stream.begin() + 8 + static_cast<std::ptrdiff_t>(16 * q));
    putU64(chain, 7ull << 28); // END, qwc = 0
    chain.insert(chain.end(), stream.begin() + 8 + static_cast<std::ptrdiff_t>(16 * q),
                 stream.end());
    return chain;
}

// One triangle batch through the entry-6 vertex layout (stride 3: position,
// colour, st; triangle-list primitive), MSCAL 12.
Bytes triangleStream() {
    Bytes s;
    putU32(s, 0x6C010000u);
    putU32(s, 3u);            // NLOOP = 3
    putU32(s, 3u << 15);      // PRIM = triangle list
    putU32(s, 0u);
    putU32(s, 0u);
    putU32(s, 0u);
    putU32(s, 0x68090000u);

    const float pos[3][3] = {{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}};
    const float col[3][3] = {{255.0f, 0.0f, 0.0f}, {0.0f, 255.0f, 0.0f}, {0.0f, 0.0f, 255.0f}};
    const float st[3][3] = {{0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}};
    for (int i = 0; i < 3; ++i) {
        for (int k = 0; k < 3; ++k) putFloat(s, pos[i][k]);
        for (int k = 0; k < 3; ++k) putFloat(s, col[i][k]);
        for (int k = 0; k < 3; ++k) putFloat(s, st[i][k]);
    }
    putU32(s, 0x1400000Cu);
    return s;
}

// One car model: 16-byte header {partA off 0, partB off 0, cntA 0, cntB 0}
// (both part gates closed) followed by the main chain.
Bytes buildModel(const Bytes& mainChain) {
    Bytes m;
    for (int i = 0; i < 4; ++i) {
        putU32(m, 0u);
    }
    m.insert(m.end(), mainChain.begin(), mainChain.end());
    return m;
}

// ---------------------------------------------------------------------------
// Livery texture-chain fixture, reproducing the documented disc shape:
// frame 1 = CNT qwc 1030 with the PSMT8 128x128 upload (DBP 0, DBW 2);
// frame 2 = CNT qwc 70 with the PSMCT32 16x16 256-entry palette (DBP 64,
// DBW 1); END tag; 0x20 trailing bytes inside the 0x4510 region. Tool-junk
// values sit exactly in the bits the build tool's pointers occupy on disc —
// the fields the GIF masks away — pinning that they are ignored. Parsed
// fields (NLOOP, FLG, NREG, the A+D address low byte, DBP/DBW/PSM, DSAX/,
// DSAY, RRW/RRH, DIR) stay clean by construction.
// ---------------------------------------------------------------------------

constexpr std::uint64_t kAddrBitbltbuf = 0x50;
constexpr std::uint64_t kAddrTrxpos = 0x51;
constexpr std::uint64_t kAddrTrxreg = 0x52;
constexpr std::uint64_t kAddrTrxdir = 0x53;

constexpr std::uint32_t kPsmCt32 = 0x00;
constexpr std::uint32_t kPsmT8 = 0x13;

// Raw 32-bit palette entry for CLUT index e: entry 0 is the all-zero entry
// (exports alpha 0); every other entry is opaque with the TEXA alpha 0x80.
// A synthetic ramp derived from the decode math, not from game data.
std::uint32_t palRaw(std::uint32_t e) {
    if (e == 0) {
        return 0;
    }
    const std::uint32_t r = e & 0xFFu;
    const std::uint32_t g = (e * 3 + 11) & 0xFFu;
    const std::uint32_t b = (e * 7 + 5) & 0xFFu;
    const std::uint32_t a = 0x80u;
    return r | (g << 8) | (b << 16) | (a << 24);
}

// CSM1 load order: bits 3 and 4 of the 8-bit CLUT entry index are swapped
// (an involution). A 16x16 CT32 palette payload stores entry e at texel
// position swizzle(e).
std::uint32_t csm1Swizzle(std::uint32_t e) {
    return (e & ~0x18u) | ((e & 0x08u) << 1) | ((e & 0x10u) >> 1);
}

Bytes palettePayloadCt32_16x16() {
    Bytes pixels(256 * 4, 0x00);
    for (std::uint32_t e = 0; e < 256; ++e) {
        putU32At(pixels, csm1Swizzle(e) * 4, palRaw(e));
    }
    return pixels;
}

// T8 texture payload: texel i's index is i % 256 (exercises entry 0 and
// every palette slot).
Bytes t8TexturePayload() {
    Bytes pixels(128 * 128, 0x00);
    for (std::size_t i = 0; i < pixels.size(); ++i) {
        pixels[i] = static_cast<std::uint8_t>(i % 256u);
    }
    return pixels;
}

// One upload frame payload: the A+D GIF packet header (NLOOP = 4,
// NREG = 1, A+D descriptor nibble 0xE — junk in every bit the GIF masks
// away, exactly where the disc's build tool stores host pointers) + the
// four upload registers + an IMAGE tag + the pixels.
Bytes uploadFramePayload(std::uint32_t dbp, std::uint32_t dbw, std::uint32_t psm,
                         std::uint32_t w, std::uint32_t h, const Bytes& pixels) {
    Bytes payload;
    // A+D packet tag qword: NLOOP = 4 | NREG = 1 at bits 60..63. Junk lives
    // in bits 16..47 (disc stores a 0x1012b9a4-class pointer there).
    putU64(payload, 4u | (1ull << 60) | (0xDEADBEEFull << 16));
    // A+D regs qword: the low nibble 0xE is what is read; junk above it.
    putU64(payload, 0xEu | (0x77ull << 8));

    // BITBLTBUF: DBP bits 32..45, DBW bits 48..53, DPSM bits 56..61; the
    // source-side bits 0..31 are masked away and carry junk on disc.
    const std::uint64_t blitbuf = (static_cast<std::uint64_t>(dbp) << 32) |
                                  (static_cast<std::uint64_t>(dbw) << 48) |
                                  (static_cast<std::uint64_t>(psm) << 56) |
                                  0xBEEF0042ull;
    // TRXPOS: the reference reads DSAX bits 32..42 and DSAY bits 48..58 —
    // both zero here; junk sits in the masked-away bits below 32 and in
    // 40..47 (the disc stores a pointer that lands around bit 46).
    const std::uint64_t trxpos = (0x1234ull << 16) | (0xC0ull << 40);
    // TRXREG: RRW bits 0..11, RRH bits 32..43; junk elsewhere.
    const std::uint64_t trxreg = static_cast<std::uint64_t>(w) |
                                 (static_cast<std::uint64_t>(h) << 32) |
                                 (0xABull << 44);
    // TRXDIR: value & 3; 0 = host-to-local. Junk above the mask.
    const std::uint64_t trxdir = (0xF00Dull << 16);

    struct Write {
        std::uint64_t value;
        std::uint64_t address;
    };
    const Write writes[4] = {
        {blitbuf, (0x42ull << 40) | kAddrBitbltbuf},
        {trxpos, (0x42ull << 40) | kAddrTrxpos},
        {trxreg, (0x42ull << 40) | kAddrTrxreg},
        {trxdir, (0x42ull << 40) | kAddrTrxdir},
    };
    for (const Write& wr : writes) {
        // A+D: value qword then address qword; only the address's low byte
        // is read, and the high bytes carry junk on disc.
        putU64(payload, wr.value);
        putU64(payload, wr.address);
    }

    // IMAGE tag: FLG = 2 at bits 58..59, NLOOP = pixel qwords; junk in the
    // EOP/PRIM bits (32..47), and the tag's upper qword is ignored junk.
    const std::uint64_t qwords = (static_cast<std::uint64_t>(pixels.size()) + 15) / 16;
    putU64(payload, qwords | (2ull << 58) | (0xF9F0ull << 32));
    putU64(payload, 0xF9F01234F9F01234ull);
    payload.insert(payload.end(), pixels.begin(), pixels.end());
    payload.resize((payload.size() + 15) & ~std::size_t{15}, 0xF0u); // keep 16-aligned
    return payload;
}

void appendDmaFrame(Bytes& chain, std::uint16_t qwc, const Bytes& payload) {
    putU16(chain, qwc);
    chain.push_back(0x00);
    chain.push_back(0x10u); // CNT
    chain.resize(chain.size() + 12, 0x5Au); // tag qwords 2..3: ignored junk
    chain.insert(chain.end(), payload.begin(), payload.end());
}

// The whole livery region: exactly 0x4510 bytes, the same frame layout as
// every disc Q-file (CNT 1030, CNT 70, END, 0x20 trailing bytes).
Bytes carTextureChain() {
    const Bytes texPayload = uploadFramePayload(0, 2, kPsmT8, 128, 128, t8TexturePayload());
    const Bytes palPayload =
        uploadFramePayload(64, 1, kPsmCt32, 16, 16, palettePayloadCt32_16x16());
    check(texPayload.size() == 1030u * 16u,
          "livery fixture: texture frame payload is qwc 1030 (disc shape)");
    check(palPayload.size() == 70u * 16u,
          "livery fixture: palette frame payload is qwc 70 (disc shape)");

    Bytes chain;
    appendDmaFrame(chain, 1030, texPayload);
    appendDmaFrame(chain, 70, palPayload);
    // END tag (byte 3 == 0x70; the walk stops here and never enters a
    // payload), then the 0x20 trailing bytes the disc region carries.
    putU16(chain, 2); // the disc END tag's qwc value
    chain.push_back(0x00);
    chain.push_back(0x70u);
    chain.resize(chain.size() + 12, 0x6Du);
    chain.resize(chain.size() + 0x20, 0xABu);
    check(chain.size() == 0x4510u, "livery fixture: region is exactly 0x4510 bytes");
    return chain;
}

// Expected decoded livery: 128x128 R8G8B8A8, byte0 = R, alpha exported with
// min(255, a * 255 / 128) (entry 0 exports alpha 0).
Bytes expectedLiveryRgba() {
    Bytes out;
    out.reserve(128 * 128 * 4);
    for (std::size_t i = 0; i < 128 * 128; ++i) {
        const std::uint32_t entry = static_cast<std::uint32_t>(i % 256u);
        const std::uint32_t raw = palRaw(entry);
        const std::uint32_t a = (raw >> 24) & 0xFFu;
        out.push_back(static_cast<std::uint8_t>(raw & 0xFFu));
        out.push_back(static_cast<std::uint8_t>((raw >> 8) & 0xFFu));
        out.push_back(static_cast<std::uint8_t>((raw >> 16) & 0xFFu));
        out.push_back(static_cast<std::uint8_t>(std::min(255u, a * 255u / 128u)));
    }
    return out;
}

// ---------------------------------------------------------------------------
// Synthetic Q-file: 12-word header, six models, optional livery region.
// ---------------------------------------------------------------------------

// Six models: model 0 carries the triangle main chain, models 1..5 carry
// empty RET-only chains.
std::vector<Bytes> makeModels() {
    const Bytes tri = dmaChain(triangleStream());
    const Bytes empty = dmaChain({});
    std::vector<Bytes> models;
    models.push_back(buildModel(tri));
    for (std::size_t k = 1; k < kCarModelCount; ++k) {
        models.push_back(buildModel(empty));
    }
    return models;
}

// Assembles a Q-file: 0x30-byte header (words 0..5 model starts, [6] the
// chain start, [7] the size, words 8..11 zero), models, then the chain. An
// empty chain builds the no-chain variant (word 6 == word 7).
Bytes makeCarFile(const std::vector<Bytes>& models, const Bytes& chain) {
    Bytes car(kCarHeaderBytes, 0x00);
    std::vector<std::uint32_t> starts;
    for (const Bytes& m : models) {
        starts.push_back(static_cast<std::uint32_t>(car.size()));
        car.insert(car.end(), m.begin(), m.end());
    }
    const std::uint32_t chainStart = static_cast<std::uint32_t>(car.size());
    car.insert(car.end(), chain.begin(), chain.end());
    for (std::size_t k = 0; k < starts.size(); ++k) {
        setU32(car, 4 * k, starts[k]);
    }
    setU32(car, 4 * kCarModelCount, chainStart);     // word 6: chain start
    setU32(car, 4 * kCarModelCount + 4,             // word 7: file size
           static_cast<std::uint32_t>(car.size()));
    check(getU32(car, 4 * kCarModelCount + 4) == car.size(),
          "car fixture: word 7 equals the assembled size");
    return car;
}

Bytes makeLiveryCar() {
    return makeCarFile(makeModels(), carTextureChain());
}

std::size_t fixtureChainStart(const Bytes& car) {
    return getU32(car, 4 * kCarModelCount);
}

// ---------------------------------------------------------------------------
// Mesh / image fixtures for the binding stage.
// ---------------------------------------------------------------------------
MeshRecord makeMesh(std::size_t vertexCount, std::uint32_t staleImageIndex) {
    MeshRecord mesh{};
    mesh.vertices.resize(vertexCount);
    for (std::size_t i = 0; i < vertexCount; ++i) {
        mesh.vertices[i].px = static_cast<float>(i);
        mesh.vertices[i].u = static_cast<float>(i) * 0.5f;
    }
    mesh.indices.resize(vertexCount);
    for (std::size_t i = 0; i < vertexCount; ++i) {
        mesh.indices[i] = static_cast<std::uint32_t>(i);
    }
    mesh.materials.push_back(
        MaterialRef{.firstIndex = 0,
                    .indexCount = static_cast<std::uint64_t>(vertexCount),
                    .imageIndex = staleImageIndex});
    Provenance provenance{};
    provenance.sourceFile = kSourceFile;
    provenance.offset = 0x10;
    provenance.length = 0x20;
    provenance.sha256 = testDigest();
    mesh.provenance = provenance;
    return mesh;
}

ImageRecord makeImage() {
    ImageRecord image{};
    image.width = 128;
    image.height = 128;
    image.mipCount = 1;
    image.format = PixelFormat::R8G8B8A8_UNORM;
    image.pixels = expectedLiveryRgba();
    image.sourceVariantId = "livery";
    Provenance provenance{};
    provenance.sourceFile = kSourceFile;
    provenance.offset = 0x4567;
    provenance.length = 0x4510;
    provenance.sha256 = testDigest();
    image.provenance = provenance;
    return image;
}

bool anySentinel(const HostAssetContainer& container) {
    for (const MeshRecord& mesh : container.meshes) {
        for (const MaterialRef& material : mesh.materials) {
            if (material.imageIndex == 0xFFFFFFFFu) {
                return true;
            }
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// (a1) parseCarTableE
// ---------------------------------------------------------------------------
void testParseCarTableE() {
    const Bytes elf = makeElf();
    check(elf.size() == elfFixtureSize(), "table E: fixture spans the whole table");
    const CarTableResult table = parseCarTableE(elf);
    check(table.err == CarTableError::Ok, "table E: accepts the synthetic ELF image");
    check(table.errorRecord == 0, "table E: no error record on success");
    check(table.records.size() == kCarRecordCount, "table E: 151 records");
    if (table.err == CarTableError::Ok) {
        bool allMatch = true;
        std::vector<std::uint32_t> lbas;
        for (std::size_t i = 0; i < kCarRecordCount; ++i) {
            if (table.records[i].sectorOffset != fixtureLba(i) ||
                table.records[i].sectorCount != fixtureSectors(i)) {
                allMatch = false;
            }
            lbas.push_back(table.records[i].sectorOffset);
        }
        check(allMatch, "table E: every record carries its sector offset and count");
        std::sort(lbas.begin(), lbas.end());
        check(std::adjacent_find(lbas.begin(), lbas.end()) == lbas.end(),
              "table E: records stay distinct (record i is car i, not run-length)");
        check(table.records[kCarRecordCount - 1].sectorOffset == fixtureLba(150),
              "table E: record 150 (Q150) reads its own fields");
    }
}

void testParseCarTableEBadTag() {
    Bytes elf = makeElf();
    const std::size_t bad =
        static_cast<std::size_t>(kCarTableElfFileOffset) + 37 * kCarDescriptorRecordBytes;
    setU32(elf, bad, 0x333488u); // tag off by one nibble
    const CarTableResult table = parseCarTableE(elf);
    check(table.err == CarTableError::BadTag, "table E: rejects a bad tag word");
    check(table.errorRecord == 37, "table E: error record locates the bad tag");
    check(table.records[0].sectorOffset == 0 && table.records[0].sectorCount == 0,
          "table E: records stay zeroed on error");
    check(table.records[kCarRecordCount - 1].sectorOffset == 0,
          "table E: later records are not filled past the failure");
}

void testParseCarTableETruncated() {
    const Bytes elf = makeElf();
    const CarTableResult whole = parseCarTableE(elf);
    check(whole.err == CarTableError::Ok, "table E: exact-fit ELF image parses");

    const Bytes oneShort(elf.begin(), elf.end() - 1);
    const CarTableResult cut = parseCarTableE(oneShort);
    check(cut.err == CarTableError::Truncated, "table E: one byte short rejects");
    check(cut.records.front().sectorOffset == 0, "table E: truncated result is empty");

    const CarTableResult empty = parseCarTableE({});
    check(empty.err == CarTableError::Truncated, "table E: empty span rejects");

    const Bytes head(elf.begin(),
                     elf.begin() + static_cast<std::ptrdiff_t>(kCarTableElfFileOffset));
    const CarTableResult headTable = parseCarTableE(head);
    check(headTable.err == CarTableError::Truncated,
          "table E: span ending at the table rejects");
}

// ---------------------------------------------------------------------------
// (a2) parseCarHeader
// ---------------------------------------------------------------------------
void testParseCarHeaderValid() {
    const Bytes car = makeLiveryCar();
    const Bytes chainless = makeCarFile(makeModels(), {});
    const CarHeaderResult r = parseCarHeader(car);
    check(r.err == CarHeaderError::Ok, "header: accepts the 12-word grammar");
    if (r.err != CarHeaderError::Ok) {
        return;
    }
    check(r.info.fileSize == car.size(), "header: file size word exposed");
    check(r.info.textureChainStart == fixtureChainStart(car),
          "header: texture-chain start word exposed");
    check(r.info.textureChainStart + 0x4510 == car.size(),
          "header: chain region is the trailing 0x4510 bytes");
    check(r.info.modelStarts[0] == kCarHeaderBytes,
          "header: model 0 starts right after the 0x30-byte header");
    bool startsMatch = true;
    for (std::size_t k = 0; k < kCarModelCount; ++k) {
        if (r.info.modelStarts[k] != getU32(car, 4 * k)) {
            startsMatch = false;
        }
    }
    check(startsMatch, "header: six model starts exposed verbatim");

    const CarHeaderResult noChain = parseCarHeader(chainless);
    check(noChain.err == CarHeaderError::Ok,
          "header: empty texture chain (word 6 == word 7) is not a header error");
    check(noChain.info.textureChainStart == chainless.size(),
          "header: chainless file names EOF as its chain start");
}

void testParseCarHeaderNegatives() {
    // Fewer than 0x30 bytes.
    {
        const Bytes shortFile(kCarHeaderBytes - 1, 0x00);
        const CarHeaderResult r = parseCarHeader(shortFile);
        check(r.err == CarHeaderError::Truncated, "header: short file rejects");
        check(r.errorOffset == 0, "header: short file carries no word offset");
        check(r.info.fileSize == 0 && r.info.textureChainStart == 0,
              "header: short file info stays default");
    }
    // Size word mismatch.
    {
        Bytes car = makeLiveryCar();
        setU32(car, 4 * kCarModelCount + 4, getU32(car, 4 * kCarModelCount + 4) + 1);
        const CarHeaderResult r = parseCarHeader(car);
        check(r.err == CarHeaderError::BadHeader, "header: size word must equal byte count");
        check(r.errorOffset == 0x1C, "header: size error locates word 7");
    }
    // Loader gate: word 6 above 0x34000.
    {
        Bytes car = makeLiveryCar();
        setU32(car, 4 * kCarModelCount, kCarModelRegionMaxBytes + 1);
        const CarHeaderResult r = parseCarHeader(car);
        check(r.err == CarHeaderError::BadHeader, "header: 0x34000 region gate enforced");
        check(r.errorOffset == 0x18, "header: gate error locates word 6");
    }
    // Model 0 overlapping the 12-word header (the 8-word-header shape).
    {
        Bytes car = makeLiveryCar();
        setU32(car, 0, 0x20);
        const CarHeaderResult r = parseCarHeader(car);
        check(r.err == CarHeaderError::BadHeader, "header: model 0 must start >= 0x30");
        check(r.errorOffset == 0, "header: model-start error locates word 0");
    }
    // Non-monotonic model starts.
    {
        Bytes car = makeLiveryCar();
        setU32(car, 8, getU32(car, 4) - 16); // word 2 < word 1
        const CarHeaderResult r = parseCarHeader(car);
        check(r.err == CarHeaderError::BadHeader, "header: model starts must not decrease");
        check(r.errorOffset == 8, "header: monotonic error locates word 2");
    }
    // Nonzero padding words.
    {
        Bytes car = makeLiveryCar();
        setU32(car, 4 * 9, 1);
        const CarHeaderResult r = parseCarHeader(car);
        check(r.err == CarHeaderError::BadHeader, "header: words 8..11 must be zero");
        check(r.errorOffset == 0x24, "header: padding error locates word 9");
    }
    // Texture chain not starting with a CNT tag.
    {
        Bytes car = makeLiveryCar();
        const std::size_t chainStart = fixtureChainStart(car);
        car[chainStart + 3] = 0x70u; // END where the loader's walk wants CNT
        const CarHeaderResult r = parseCarHeader(car);
        check(r.err == CarHeaderError::BadHeader,
              "header: claimed chain must begin with a CNT tag");
        check(r.errorOffset == chainStart, "header: chain error locates the chain start");
    }
    // Chain claimed with no room left for its first DMA tag.
    {
        Bytes car = makeCarFile(makeModels(), {});
        const std::size_t chainStart = car.size();
        setU32(car, 4 * kCarModelCount,
               static_cast<std::uint32_t>(chainStart - 8)); // word 6 = EOF - 8
        const CarHeaderResult r = parseCarHeader(car);
        check(r.err == CarHeaderError::BadHeader,
              "header: chain start without room for a tag rejects");
        check(r.errorOffset == chainStart - 8,
              "header: short-chain error locates the claimed start");
    }
}

// ---------------------------------------------------------------------------
// (b) extractCarLivery
// ---------------------------------------------------------------------------
Provenance liveryProvenance() {
    Provenance provenance{};
    provenance.sourceFile = kSourceFile;
    provenance.offset = 0x4567;
    provenance.length = 0x4510;
    provenance.sha256 = testDigest();
    return provenance;
}

void testExtractCarLiveryValid() {
    const Bytes car = makeLiveryCar();
    const Provenance provenance = liveryProvenance();

    const CarLiveryResult livery = extractCarLivery(car, provenance, "livery");
    check(livery.err == CarLiveryError::Ok, "livery: accepts the disc-shaped chain");
    check(livery.chainStart == fixtureChainStart(car), "livery: chain start exposed");
    if (livery.err != CarLiveryError::Ok) {
        return;
    }
    check(livery.image.width == 128 && livery.image.height == 128,
          "livery: 128x128 PSMT8 texture decodes at full size");
    check(livery.image.mipCount == 1, "livery: single mip level");
    check(livery.image.format == PixelFormat::R8G8B8A8_UNORM, "livery: RGBA8 format");
    check(livery.image.pixels == expectedLiveryRgba(),
          "livery: pixels equal the decode-math expectation");
    check(livery.image.provenance == provenance, "livery: provenance passthrough verbatim");
    check(livery.image.sourceVariantId == "livery", "livery: variant id passthrough");

    // Exactly one palette selection for a PSMT8 + single CT32 chain.
    const std::span<const std::uint8_t> chain =
        std::span<const std::uint8_t>(car).subspan(fixtureChainStart(car));
    check(convertImageBankCount(chain) == 1,
          "livery: single palette selection (bank count 1)");
}

void testExtractCarLiveryNegatives() {
    const Bytes car = makeLiveryCar();
    const Provenance provenance = liveryProvenance();

    // Header rejected first.
    {
        Bytes broken = car;
        setU32(broken, 4 * kCarModelCount + 4, getU32(broken, 4 * kCarModelCount + 4) + 1);
        const CarLiveryResult r = extractCarLivery(broken, provenance, "livery");
        check(r.err == CarLiveryError::BadHeader, "livery: bad header surfaces BadHeader");
        check(r.errorOffset == 0x1C, "livery: bad header carries its word offset");
        check(r.image == ImageRecord{}, "livery: bad header yields an empty record");
    }
    // Header fine (first tag CNT), but the chain's payload claim runs past
    // the file: the image pipeline rejects, so BadChain.
    {
        Bytes junkChain;
        putU16(junkChain, 0xFFFF); // claims ~1 MB of payload
        junkChain.push_back(0x00);
        junkChain.push_back(0x10u); // CNT, so the header's chain check passes
        junkChain.resize(junkChain.size() + 12, 0x00);
        const Bytes broken = makeCarFile(makeModels(), junkChain);
        const CarLiveryResult r = extractCarLivery(broken, provenance, "livery");
        check(r.err == CarLiveryError::BadChain,
              "livery: unwalkable chain surfaces BadChain");
        check(r.chainStart == fixtureChainStart(broken),
              "livery: BadChain still names the chain start");
        check(r.image == ImageRecord{}, "livery: BadChain yields an empty record");
    }
    // Zero-length chain: the header accepts (no claim), the pipeline rejects.
    {
        const Bytes chainless = makeCarFile(makeModels(), {});
        const CarLiveryResult r = extractCarLivery(chainless, provenance, "livery");
        check(r.err == CarLiveryError::BadChain,
              "livery: a file with no chain region has no livery");
        check(r.image == ImageRecord{}, "livery: chainless result is empty");
    }
}

// ---------------------------------------------------------------------------
// (c) bindCarModels
// ---------------------------------------------------------------------------
void testBindCarModels() {
    const ImageRecord image = makeImage();
    const std::span<const ImageRecord> images(&image, 1);

    // Two meshes, one keeping the convertCarModels sentinel and one carrying
    // a stale resolved index: both must end at the requested index, and the
    // inputs must not gain side effects.
    MeshRecord sentinelMesh = makeMesh(3, 0xFFFFFFFFu);
    MeshRecord staleMesh = makeMesh(4, 7u);
    const MeshRecord sentinelBefore = sentinelMesh;
    const MeshRecord staleBefore = staleMesh;
    const MeshRecord meshes[2] = {sentinelMesh, staleMesh};

    const CarBindResult bound = bindCarModels(std::span<const MeshRecord>(meshes), images, 0);
    check(bound.err == CarBindError::Ok, "bind: resolves two meshes onto one image");
    check(bound.models.size() == 2, "bind: one model per mesh");
    if (bound.err == CarBindError::Ok && bound.models.size() == 2) {
        check(bound.models[0].materialImage == std::vector<std::uint32_t>{0},
              "bind: sentinel material resolves to the livery index");
        check(bound.models[1].materialImage == std::vector<std::uint32_t>{0},
              "bind: stale material index is replaced, not kept");
        check(bound.models[0].vertices == sentinelMesh.vertices &&
                  bound.models[0].indices == sentinelMesh.indices,
              "bind: vertices and indices are copied verbatim");
        check(bound.models[1].vertices == staleMesh.vertices,
              "bind: second mesh geometry is copied verbatim");
    }
    check(sentinelMesh == sentinelBefore, "bind: input meshes are not modified (sentinel)");
    check(staleMesh == staleBefore, "bind: input meshes are not modified (stale)");

    // A nonzero livery index within range resolves there too.
    const ImageRecord imagePair[2] = {makeImage(), makeImage()};
    const CarBindResult indexOne = bindCarModels(
        std::span<const MeshRecord>(meshes), std::span<const ImageRecord>(imagePair, 2), 1);
    check(indexOne.err == CarBindError::Ok, "bind: a second image index is legal");
    check(indexOne.models.size() == 2 &&
              indexOne.models[0].materialImage == std::vector<std::uint32_t>{1},
          "bind: materials resolve to the requested index 1");

    // Zero meshes is a legal no-op bind.
    const CarBindResult none = bindCarModels({}, images, 0);
    check(none.err == CarBindError::Ok, "bind: no meshes is Ok");
    check(none.models.empty(), "bind: no meshes yields no models");

    // Empty images / out-of-range index.
    const CarBindResult noImages = bindCarModels(std::span<const MeshRecord>(meshes), {}, 0);
    check(noImages.err == CarBindError::NoImages, "bind: no images rejects");
    const CarBindResult badIndex = bindCarModels(std::span<const MeshRecord>(meshes), images, 1);
    check(badIndex.err == CarBindError::BadImageIndex,
          "bind: index at the image count rejects");

    // loadModel failure (an index outside its own vertices) maps to
    // LoaderError and names the mesh.
    {
        MeshRecord good = makeMesh(2, 0xFFFFFFFFu);
        MeshRecord bad = makeMesh(2, 0xFFFFFFFFu);
        bad.indices[1] = 2; // == vertex count
        const MeshRecord failing[2] = {good, bad};
        const CarBindResult r = bindCarModels(std::span<const MeshRecord>(failing), images, 0);
        check(r.err == CarBindError::LoaderError, "bind: a rejected mesh surfaces LoaderError");
        check(r.errorMesh == 1, "bind: the rejected mesh index is named");
        check(r.models.empty(), "bind: models stay empty on error");
    }
}

// ---------------------------------------------------------------------------
// Composite loadCarAssets
// ---------------------------------------------------------------------------
void testLoadCarAssetsValid() {
    const Bytes car = makeLiveryCar();
    const CarAssetResult result = loadCarAssets(car, kSourceFile, testDigest());
    check(result.err == CarAssetError::Ok, "composite: whole car loads");
    check(result.headerError == CarHeaderError::Ok && result.modelError == CarModelError::Ok &&
              result.liveryError == CarLiveryError::Ok && result.bindError == CarBindError::Ok,
          "composite: all stage details stay Ok on success");
    if (result.err != CarAssetError::Ok) {
        return;
    }

    const HostAssetContainer& container = result.container;
    check(container.version == kContainerVersion, "composite: container version set");
    check(container.meshes.size() == kCarModelCount,
          "composite: six model meshes converted");
    check(container.images.size() == 1, "composite: exactly one livery image");

    // The livery record: the decode carries the composite's own provenance
    // (offset = chain start, length = chain region) and variant "livery".
    const ImageRecord& livery = container.images.front();
    check(livery.width == 128 && livery.height == 128 && livery.mipCount == 1,
          "composite: livery record shape");
    check(livery.pixels == expectedLiveryRgba(), "composite: livery pixels decoded");
    check(livery.sourceVariantId == "livery", "composite: livery variant id");
    Provenance expectProvenance{};
    expectProvenance.sourceFile = kSourceFile;
    expectProvenance.offset = fixtureChainStart(car);
    expectProvenance.length = 0x4510;
    expectProvenance.sha256 = testDigest();
    check(livery.provenance == expectProvenance,
          "composite: livery provenance names the chain span");

    check(!anySentinel(container),
          "composite: no unresolved material sentinel anywhere");
    bool allBound = true;
    for (const MeshRecord& mesh : container.meshes) {
        for (const MaterialRef& material : mesh.materials) {
            if (material.imageIndex != 0) {
                allBound = false;
            }
        }
    }
    check(allBound, "composite: every container material resolves to image 0");

    check(result.models.size() == container.meshes.size(),
          "composite: one NativeModel per mesh");
    bool modelsBound = true;
    for (std::size_t i = 0; i < result.models.size(); ++i) {
        const NativeModel& model = result.models[i];
        if (model.vertices != container.meshes[i].vertices ||
            model.indices != container.meshes[i].indices) {
            modelsBound = false;
        }
        for (std::uint32_t imageIndex : model.materialImage) {
            if (imageIndex != 0) {
                modelsBound = false;
            }
        }
    }
    check(modelsBound, "composite: every model is fully bound to image 0");
    check(!result.models.empty() && result.models[0].vertices.size() == 3,
          "composite: model 0 carries the triangle geometry");

    // The fully-bound container is wire-valid and round-trips.
    try {
        const Bytes wire = serialize(container);
        const rt::native::assets::ContainerResult back = deserialize(wire);
        check(back.err == rt::native::assets::ContainerError::Ok,
              "composite: deserialize accepts the bound container");
        check(back.value == container, "composite: round-trips to an equal container");
        check(serialize(back.value) == wire, "composite: re-serialization is byte-stable");
    } catch (const std::exception& e) {
        check(false, std::string("composite: serialize threw: ") + e.what());
    }
}

void testLoadCarAssetsNegatives() {
    const Bytes car = makeLiveryCar();

    // Provenance completeness is convertCarModels' contract; the composite
    // surfaces its error code and keeps the value empty.
    {
        const CarAssetResult r = loadCarAssets(car, "", testDigest());
        check(r.err == CarAssetError::ModelStage, "composite: empty sourceFile -> ModelStage");
        check(r.modelError == CarModelError::MissingProvenance,
              "composite: MissingProvenance detail carried");
        check(r.container.meshes.empty() && r.container.images.empty(),
              "composite: value empty on missing provenance");
    }
    {
        const Digest zero{};
        const CarAssetResult r = loadCarAssets(car, kSourceFile, zero);
        check(r.err == CarAssetError::ModelStage, "composite: zero digest -> ModelStage");
        check(r.modelError == CarModelError::MissingProvenance,
              "composite: zero digest detail carried");
    }
    // Header stage failure.
    {
        Bytes broken = car;
        setU32(broken, 4 * kCarModelCount + 4, getU32(broken, 4 * kCarModelCount + 4) + 1);
        const CarAssetResult r = loadCarAssets(broken, kSourceFile, testDigest());
        check(r.err == CarAssetError::BadHeader, "composite: header failure surfaces");
        check(r.headerError == CarHeaderError::BadHeader, "composite: header detail carried");
        check(r.errorOffset == 0x1C, "composite: header word offset carried");
        check(r.container.meshes.empty(), "composite: no meshes leak past a header failure");
    }
    // Livery stage failure on an otherwise valid file.
    {
        Bytes junkChain;
        putU16(junkChain, 0xFFFF);
        junkChain.push_back(0x00);
        junkChain.push_back(0x10u);
        junkChain.resize(junkChain.size() + 12, 0x00);
        const Bytes broken = makeCarFile(makeModels(), junkChain);
        const CarAssetResult r = loadCarAssets(broken, kSourceFile, testDigest());
        check(r.err == CarAssetError::BadLivery, "composite: bad chain surfaces BadLivery");
        check(r.liveryError == CarLiveryError::BadChain, "composite: livery detail carried");
        check(r.errorOffset == fixtureChainStart(broken),
              "composite: BadLivery error offset names the chain start");
        check(r.container.meshes.empty() && r.container.images.empty(),
              "composite: value empty on livery failure");
    }
}

} // namespace

int main() {
    testParseCarTableE();
    testParseCarTableEBadTag();
    testParseCarTableETruncated();
    testParseCarHeaderValid();
    testParseCarHeaderNegatives();
    testExtractCarLiveryValid();
    testExtractCarLiveryNegatives();
    testBindCarModels();
    testLoadCarAssetsValid();
    testLoadCarAssetsNegatives();

    if (g_failures > 0) {
        std::cerr << "test_car_models: " << g_failures << " of " << g_checks
                  << " checks FAILED\n";
        return EXIT_FAILURE;
    }
    std::cout << "test_car_models: " << g_checks << " checks passed\n";
    return EXIT_SUCCESS;
}
