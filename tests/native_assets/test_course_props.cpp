// COURSE prop-chain parser contracts (synthetic grammar bytes only; no game
// data) plus an optional local-disc validation report with the pinned
// per-course constants from docs/native-reference/course-prop-chains.md.

#include "../../src/native/assets/course_props.h"

#include <algorithm>
#include <bit>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using rt::native::assets::CoursePropError;
using rt::native::assets::parseCourseProps;

namespace {

int g_checks = 0;
int g_failures = 0;

void check(bool condition, const std::string& description) {
    ++g_checks;
    if (!condition) {
        ++g_failures;
        std::cerr << "FAIL: " << description << '\n';
    }
}

using Bytes = std::vector<std::uint8_t>;

void setU32(Bytes& bytes, std::size_t offset, std::uint32_t value) {
    for (std::size_t i = 0; i < 4; ++i) {
        bytes[offset + i] = static_cast<std::uint8_t>(value >> (8 * i));
    }
}

void setU64(Bytes& bytes, std::size_t offset, std::uint64_t value) {
    setU32(bytes, offset, static_cast<std::uint32_t>(value));
    setU32(bytes, offset + 4, static_cast<std::uint32_t>(value >> 32));
}

void setF32(Bytes& bytes, std::size_t offset, float value) {
    setU32(bytes, offset, std::bit_cast<std::uint32_t>(value));
}

constexpr std::uint32_t kVifStcycl = 0x01000101u;
constexpr std::uint32_t kVifUnpackV4 = 0x6C018000u;
constexpr std::uint32_t kVifUnpackV3Base = 0x68008001u;
constexpr std::uint32_t kVifMscalf = 0x1500000Au;
constexpr std::uint32_t kOffC = 0x20u;

std::uint64_t gifTagLo(std::uint32_t nloop, bool pre, bool eop) {
    std::uint64_t tag = nloop & 0x7FFFu;
    if (eop) tag |= 1ull << 15;
    if (pre) tag |= 1ull << 46;
    tag |= 0x264ull << 47; // PRIM: TRISTRIP + ABE, flat, untextured
    tag |= 3ull << 60;     // NREG = 3
    return tag;
}

// Handcrafts a synthetic COURSE tile around the documented prop grammar.
struct CourseFixture {
    std::uint32_t declaredN = 0;      // prologue word 3 (may differ on purpose)
    std::vector<std::uint32_t> nloops; // actual draws written to the stream
    bool flavorB = false;             // false: A (0,1,0) @ 128; true: B (0,0,1) @ 1
    std::uint32_t extraBlockBytes = 0; // opaque second packet block
};

Bytes buildCourse(const CourseFixture& fixture) {
    Bytes bytes(0x1000, 0);
    // Header: offA = offB = offC = 0x20; boundaries patched below.
    setU32(bytes, 0, 0x20);
    setU32(bytes, 4, kOffC);
    setU32(bytes, 8, kOffC);
    setU32(bytes, 12, kOffC);

    std::size_t cursor = kOffC;
    setU32(bytes, cursor + 0, 0x30);
    setU32(bytes, cursor + 4, 0x50);
    setU32(bytes, cursor + 8, 0);
    setU32(bytes, cursor + 12, fixture.declaredN);
    // Setup records A and B (the proven constant 8-tuple).
    const std::uint32_t setup[8] = {0x10000000, 0, 0x01000101, 0,
                                    0x60000000, 0, 0, 0};
    for (int record = 0; record < 2; ++record) {
        for (std::size_t i = 0; i < 8; ++i) {
            setU32(bytes, cursor + 0x10 + 0x20 * record + 4 * i, setup[i]);
        }
    }
    cursor += 0x50;

    // Byte accounting: content = 28*N + 48*V, stream = 4 + content + pad,
    // stream bytes == 8 + 16*qwc, pad in {0,4,8,12}.
    const std::uint32_t drawCount =
        static_cast<std::uint32_t>(fixture.nloops.size());
    std::uint32_t vertexTotal = 0;
    for (const std::uint32_t nloop : fixture.nloops) vertexTotal += nloop;
    const std::uint32_t content = 28 * drawCount + 48 * vertexTotal;
    const std::uint32_t qwc = (content - 4 + 15) / 16;
    const std::uint32_t pad = 16 * qwc + 4 - content;

    setU32(bytes, cursor + 0, 0x10000000u | qwc);
    setU32(bytes, cursor + 4, 0);
    setU32(bytes, cursor + 8, kVifStcycl);
    setU32(bytes, cursor + 12, kVifUnpackV4); // TTE word = first draw's UNPACK
    cursor += 0x10;

    const float normal[3] = {0.0f, fixture.flavorB ? 0.0f : 1.0f,
                             fixture.flavorB ? 1.0f : 0.0f};
    const float alpha = fixture.flavorB ? 1.0f : 128.0f;
    for (std::size_t draw = 0; draw < fixture.nloops.size(); ++draw) {
        if (draw != 0) {
            setU32(bytes, cursor, kVifUnpackV4);
            cursor += 4;
        }
        const std::uint32_t nloop = fixture.nloops[draw];
        setU64(bytes, cursor, gifTagLo(nloop, /*pre=*/true, /*eop=*/true));
        setU64(bytes, cursor + 8, 0x412); // REGS = ST, RGBAQ, XYZF2
        cursor += 16;
        setU32(bytes, cursor, kVifUnpackV3Base | (4u * nloop) << 16);
        cursor += 4;
        for (std::uint32_t vertex = 0; vertex < nloop; ++vertex) {
            const float x = 100.0f + 10.0f * static_cast<float>(draw) +
                            0.5f * static_cast<float>(vertex);
            const float z = 200.0f + 20.0f * static_cast<float>(draw) +
                            0.25f * static_cast<float>(vertex);
            setF32(bytes, cursor + 0, x);
            setF32(bytes, cursor + 4, 0.0f);
            setF32(bytes, cursor + 8, z);
            setF32(bytes, cursor + 12, normal[0]);
            setF32(bytes, cursor + 16, normal[1]);
            setF32(bytes, cursor + 20, normal[2]);
            setF32(bytes, cursor + 24, 255.0f);
            setF32(bytes, cursor + 28, 255.0f);
            setF32(bytes, cursor + 32, 255.0f);
            setF32(bytes, cursor + 36, 0.0f);
            setF32(bytes, cursor + 40, 0.0f);
            setF32(bytes, cursor + 44, alpha);
            cursor += 48;
        }
        setU32(bytes, cursor, kVifMscalf);
        cursor += 4;
    }
    // Trailing NOPs are zero words; just advance and terminate with RET.
    cursor += pad;
    setU32(bytes, cursor, 0x60000000); // RET, QWC 0
    cursor += 16;

    const std::uint32_t primaryEnd = static_cast<std::uint32_t>(cursor);
    std::uint32_t fileSize = primaryEnd + fixture.extraBlockBytes;
    setU32(bytes, 16, primaryEnd);
    if (fixture.extraBlockBytes != 0) {
        setU32(bytes, 20, fileSize);
    }
    bytes.resize(fileSize);
    return bytes;
}

Bytes buildValidChain() {
    return buildCourse({/*declaredN=*/2, /*nloops=*/{3, 4}});
}

void testValidChainDecodes() {
    const Bytes bytes = buildValidChain();
    const auto chain = parseCourseProps(bytes);
    check(chain.has_value(), "valid chain parses");
    if (!chain) return;
    check(chain->headerSize == 0x20, "header size retained");
    check(chain->propCount == 2 && chain->draws.size() == 2, "two draws decoded");
    check(chain->tailBegin == kOffC && chain->tailEnd == bytes.size(),
          "primary span covers the tail");
    check(chain->boundaries.size() == 1 && chain->boundaries[0] == bytes.size(),
          "single boundary equals file size");
    check(chain->extraBlockCount == 0, "no extra blocks");
    check(chain->draws[0].vertexCount == 3 && chain->draws[1].vertexCount == 4,
          "NLOOP honoured per draw");
    check(chain->draws[0].vertices.size() == 3 &&
              chain->draws[1].vertices.size() == 4,
          "vertex strips honoured per draw");
    check(chain->draws[0].tagOffset == kOffC + 0x60,
          "first GIFtag offset is the provenance anchor");
    check(chain->draws[1].tagOffset == kOffC + 0x5C + (28 + 48 * 3) + 4,
          "second GIFtag offset follows the byte accounting");
    check(chain->draws[0].vertices[0].x == 100.0f &&
              chain->draws[0].vertices[2].x == 101.0f,
          "x positions round-trip");
    check(chain->draws[1].vertices[1].z == 220.25f,
          "z positions round-trip per draw");
    check(chain->flavorNormal[0] == 0.0f && chain->flavorNormal[1] == 1.0f &&
              chain->flavorNormal[2] == 0.0f,
          "flavor A normal constant decoded");
    check(chain->alpha == 128.0f, "flavor A alpha decoded");
}

void testWrongTagRejection() {
    // DMA tag id REF (0x30) instead of CNT (0x10).
    Bytes refTag = buildValidChain();
    setU32(refTag, kOffC + 0x50, 0x30000000u | 25u);
    check(parseCourseProps(refTag).error() == CoursePropError::BadDmaChain,
          "non-CNT DMA tag is rejected");

    // GIFtag PRE bit clear.
    Bytes noPre = buildValidChain();
    setU64(noPre, kOffC + 0x60, gifTagLo(3, /*pre=*/false, /*eop=*/true));
    check(parseCourseProps(noPre).error() == CoursePropError::BadGifTag,
          "GIFtag without PRE is rejected");

    // REGS descriptor not 0x412.
    Bytes wrongRegs = buildValidChain();
    setU64(wrongRegs, kOffC + 0x68, 0x413);
    check(parseCourseProps(wrongRegs).error() == CoursePropError::BadGifTag,
          "wrong REGS descriptor is rejected");

    // TRILIST PRIM instead of the proven TRISTRIP shape.
    Bytes wrongPrim = buildValidChain();
    std::uint64_t tag = gifTagLo(3, true, true);
    tag = (tag & ~(0x7FFull << 47)) | (0x261ull << 47);
    setU64(wrongPrim, kOffC + 0x60, tag);
    check(parseCourseProps(wrongPrim).error() == CoursePropError::BadGifTag,
          "non-0x264 PRIM is rejected");

    // MSCALF replaced by a random word.
    Bytes badKick = buildValidChain();
    const std::size_t mscalfOffset = kOffC + 0x60 + 20 + 48 * 3;
    setU32(badKick, mscalfOffset, 0x1500000Bu); // imm 0xB, not 0xA
    check(parseCourseProps(badKick).error() == CoursePropError::BadVifStream,
          "non-prologue-kick VIF word is rejected");

    // Prologue corruption.
    Bytes badPrologue = buildValidChain();
    setU32(badPrologue, kOffC, 0x40);
    check(parseCourseProps(badPrologue).error() == CoursePropError::BadPrologue,
          "corrupt prologue is rejected");

    // Setup record corruption.
    Bytes badSetup = buildValidChain();
    setU32(badSetup, kOffC + 0x10, 0);
    check(parseCourseProps(badSetup).error() == CoursePropError::BadSetupRecord,
          "corrupt setup record is rejected");
}

void testCountMismatchDetection() {
    // Prologue says 3 draws; the stream carries only 2.
    Bytes less = buildCourse({/*declaredN=*/3, /*nloops=*/{3, 4}});
    check(parseCourseProps(less).error() == CoursePropError::BadVifStream,
          "N larger than streamed draws is rejected");

    // Prologue says 1 draw; the stream carries 2 — the leftover UNPACK word
    // is not a legal trailing NOP.
    Bytes more = buildCourse({/*declaredN=*/1, /*nloops=*/{3, 4}});
    check(parseCourseProps(more).error() == CoursePropError::BadVifStream,
          "N smaller than streamed draws is rejected");
}

void testBoundaryListExtraction() {
    // One opaque extra block after the primary block.
    Bytes bytes = buildCourse({/*declaredN=*/2, /*nloops=*/{3, 4},
                               /*flavorB=*/false, /*extraBlockBytes=*/64});
    const auto chain = parseCourseProps(bytes);
    check(chain.has_value(), "chain with extra block parses");
    if (!chain) return;
    check(chain->boundaries.size() == 2, "two boundaries extracted");
    check(chain->boundaries.back() == bytes.size(), "last boundary is EOF");
    check(chain->tailEnd == chain->boundaries[0], "primary end is boundary 0");
    check(chain->tailEnd == bytes.size() - 64,
          "extra block is skipped by size alone");
    check(chain->extraBlockCount == 1, "extra block counted, not parsed");
    check(chain->propCount == 2, "extra block does not corrupt N");

    // C16 form: offC == file size, boundary words zero — zero-length tail.
    Bytes empty(0x20, 0);
    setU32(empty, 0, 0x20);
    setU32(empty, 4, 0x20); // offA
    setU32(empty, 8, 0x20); // offB
    setU32(empty, 12, 0x20); // offC == file size
    const auto orphan = parseCourseProps(empty);
    check(orphan.has_value(), "zero-length tail parses");
    if (orphan) {
        check(orphan->propCount == 0 && orphan->draws.empty(),
              "orphan tile carries no props");
        check(orphan->boundaries.empty(), "orphan tile has no boundaries");
        check(orphan->tailBegin == orphan->tailEnd, "orphan span is empty");
    }
}

void testAlphaRoundTrip() {
    // Flavor B (C14-like): normal (0,0,1), alpha 1.
    Bytes bytes = buildCourse({/*declaredN=*/2, /*nloops=*/{4, 5},
                               /*flavorB=*/true});
    const auto chain = parseCourseProps(bytes);
    check(chain.has_value(), "flavor B chain parses");
    if (!chain) return;
    check(chain->alpha == 1.0f, "flavor B alpha decoded");
    check(chain->flavorNormal[2] == 1.0f, "flavor B normal decoded");
    bool alphas = true;
    for (const auto& draw : chain->draws) {
        for (const auto& vertex : draw.vertices) alphas &= vertex.alpha == 1;
    }
    check(alphas, "per-vertex alpha round-trips as 1");

    // Flavor A alpha rides as 128 on every vertex.
    const auto chainA = parseCourseProps(buildValidChain());
    check(chainA.has_value(), "flavor A chain parses for alpha check");
    if (chainA) {
        bool alphasA = true;
        for (const auto& draw : chainA->draws) {
            for (const auto& vertex : draw.vertices) {
                alphasA &= vertex.alpha == 128;
            }
        }
        check(alphasA, "flavor A alpha round-trips as 128");
    }

    // Corrupt vertex units must reject: y off the ground plane, and an
    // alpha that belongs to neither flavor.
    Bytes badY = buildValidChain();
    setF32(badY, kOffC + 0x60 + 20 + 4, 1.0f); // y of vertex 0
    check(parseCourseProps(badY).error() == CoursePropError::BadVertexUnit,
          "y != 0 vertex is rejected");

    Bytes badAlpha = buildValidChain();
    setF32(badAlpha, kOffC + 0x60 + 20 + 44, 64.0f);
    check(parseCourseProps(badAlpha).error() == CoursePropError::BadVertexUnit,
          "alpha out of the flavor pair is rejected");
}

void testTruncationAndHeader() {
    check(parseCourseProps(Bytes(0x1F)).error() == CoursePropError::Truncated,
          "short header is truncated");
    Bytes badSize(0x20, 0);
    setU32(badSize, 0, 0x28);
    check(parseCourseProps(badSize).error() == CoursePropError::BadHeader,
          "unsupported header size is rejected");
}

std::filesystem::path discRoot() {
    if (const char* root = std::getenv("RT_DISC")) return root;
    if (const char* home = std::getenv("HOME")) {
        return std::filesystem::path(home) / "Library/Application Support/RoadTripRecomp/disc";
    }
    return {};
}

void testRealCoursesAgainstPinnedTable() {
    const auto directory = discRoot() / "COURSE";
    if (directory.empty() || !std::filesystem::exists(directory)) {
        std::cout << "test_course_props: SKIP real COURSE directory (not found)\n";
        return;
    }

    // Pinned prologue N per tile (docs/native-reference/course-prop-chains.md).
    const std::vector<std::pair<std::string, std::uint32_t>> pinned = {
        {"C00.BIN", 12},  {"C01.BIN", 18}, {"C02.BIN", 46}, {"C03.BIN", 47},
        {"C04.BIN", 13},  {"C05.BIN", 20}, {"C06.BIN", 18}, {"C07.BIN", 15},
        {"C08.BIN", 15},  {"C09.BIN", 22}, {"C10.BIN", 14}, {"C11.BIN", 18},
        {"C12.BIN", 28},  {"C13.BIN", 9},  {"C14.BIN", 44}, {"C16.BIN", 0},
        {"C18.BIN", 102}, {"C19.BIN", 102}, {"C20.BIN", 9},
    };
    for (const auto& [name, expectedN] : pinned) {
        const auto path = directory / name;
        if (!std::filesystem::exists(path)) {
            ++g_failures;
            std::cerr << "FAIL: " << name << " missing from disc root\n";
            continue;
        }
        std::ifstream input(path, std::ios::binary);
        const Bytes bytes((std::istreambuf_iterator<char>(input)),
                          std::istreambuf_iterator<char>());
        const auto chain = parseCourseProps(bytes);
        check(chain.has_value(), name + ": prop chain parses");
        if (!chain) continue;
        check(chain->propCount == expectedN, name + ": N matches the pin");
        std::uint32_t vertices = 0;
        for (const auto& draw : chain->draws) vertices += draw.vertexCount;
        const std::uint32_t qwc =
            expectedN == 0 ? 0
                           : (chain->tailEnd - chain->tailBegin - 112) / 16;
        std::cout << "test_course_props: " << name << " N=" << chain->propCount
                  << " boundaries=" << chain->boundaries.size()
                  << " strips=" << chain->draws.size()
                  << " vertices=" << vertices << " qwc=" << qwc
                  << " extras=" << chain->extraBlockCount
                  << " alpha=" << chain->alpha << '\n';
    }
}

} // namespace

int main() {
    testValidChainDecodes();
    testWrongTagRejection();
    testCountMismatchDetection();
    testBoundaryListExtraction();
    testAlphaRoundTrip();
    testTruncationAndHeader();
    testRealCoursesAgainstPinnedTable();
    std::cout << "test_course_props: " << (g_checks - g_failures) << '/' << g_checks
              << " checks passed\n";
    return g_failures == 0 ? 0 : 1;
}
