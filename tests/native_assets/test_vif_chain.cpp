// tests/native_assets/test_vif_chain.cpp
//
// Synthetic contract tests for the strict VIF/DMA chain decoder.
// Decoder under test: src/native/assets/vif_chain.cpp (header: vif_chain.h).
// Frozen dependency: src/native/assets/container.h (the Vertex type only).
//
// Every chain byte is assembled by the independent little-endian encoder below.
// decodeVifChain() under test is only exercised on synthetic, hand-built chains.
// No game bytes, no disc bytes, no duplicated types, no mocks.
//
// check() is a plain counter + if-statement (no assert()): these checks run
// under NDEBUG.
//
// NOT RUN: compiling, linking and executing this file is not done in this task;
// the contract under test (vif_chain.cpp) is the production implementation.

#include "../../src/native/assets/vif_chain.h"
#include "../../src/native/assets/container.h"

#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <limits>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

using rt::native::assets::DecodedBatch;
using rt::native::assets::Vertex;
using rt::native::assets::VifChainError;
using rt::native::assets::VifChainResult;
using rt::native::assets::decodeVifChain;

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

const char* err_name(VifChainError::Code e) {
    switch (e) {
        case VifChainError::Code::Ok:        return "Ok";
        case VifChainError::Code::Truncated: return "Truncated";
        case VifChainError::Code::Overflow:  return "Overflow";
        case VifChainError::Code::BadTag:    return "BadTag";
    }
    return "VifChainError(unknown)";
}

// Decode a synthetic chain. Exceptions are treated as test failures (the strict
// decoder is documented to answer through the structured error union, never to
// throw).
VifChainResult run_decode(const std::vector<std::uint8_t>& bytes, const std::string& what) {
    try {
        return decodeVifChain(bytes);
    } catch (const std::exception& e) {
        check(false, what + ": decodeVifChain threw: " + e.what());
    } catch (...) {
        check(false, what + ": decodeVifChain threw a non-std exception");
    }
    return VifChainResult{};
}

// A failed decode must carry the wanted code AND no partial batches (decoder
// comment: "returns error (no partial batches) on malformed/truncated/overflow
// input"). The vector-of-DecodedBatch assertion enforces the second half.
void expect_err(const std::vector<std::uint8_t>& bytes, VifChainError::Code want,
                const std::string& what) {
    VifChainResult r = run_decode(bytes, what);
    check(r.error == want,
          what + ": expected " + err_name(want) + ", got " + err_name(r.error));
    check(r.batches.empty(),
          what + ": failed decode carries no partial batches");
}

// ---------------------------------------------------------------------------
// Independent little-endian encoder (frozen wire contract; never raw struct
// layouts). Used only to author fixture bytes by hand.
// ---------------------------------------------------------------------------
void put_u32(std::vector<std::uint8_t>& out, std::uint32_t v) {
    out.push_back(static_cast<std::uint8_t>(v & 0xFFu));
    out.push_back(static_cast<std::uint8_t>((v >> 8) & 0xFFu));
    out.push_back(static_cast<std::uint8_t>((v >> 16) & 0xFFu));
    out.push_back(static_cast<std::uint8_t>((v >> 24) & 0xFFu));
}

[[maybe_unused]] void put_f32(std::vector<std::uint8_t>& out, float v) {
    put_u32(out, std::bit_cast<std::uint32_t>(v));
}

// ---------------------------------------------------------------------------
// ChainBuilder: writes the DMA tag in its on-the-wire order. The strict decoder
// (src/native/assets/vif_chain.cpp) walks a DMA chain the same way
// tools/model_dump.cpp does: every tag contributes its upper 8 bytes (the VIF
// code qword) followed by qwc*16 bytes of payload. The VIF command is the
// low 4 bytes of that qword; the next 4 bytes (the "VIF pad") are read as the
// first data word by the UNPACK family (this is how the strict decoder builds
// cur.words[0] / cur.hdr[0]).
//
// Wire format reminder (one DMA tag, 16 bytes header + qwc*16 bytes payload):
//   bytes 0..7   : lo = qwc | (id << 28) | (addr << 32); addr ignored for CNT
//   bytes 8..11  : VIF command (cmd<<24 | num<<16 | imm)
//   bytes 12..15 : VIF pad; for UNPACK becomes the first u32 of the payload
//   bytes 16..   : qwc * 16 bytes payload
// ---------------------------------------------------------------------------
struct ChainBuilder {
    std::vector<std::uint8_t> bytes;

    void put_u32(std::uint32_t v) {
        bytes.push_back(static_cast<std::uint8_t>(v & 0xFFu));
        bytes.push_back(static_cast<std::uint8_t>((v >> 8) & 0xFFu));
        bytes.push_back(static_cast<std::uint8_t>((v >> 16) & 0xFFu));
        bytes.push_back(static_cast<std::uint8_t>((v >> 24) & 0xFFu));
    }

    // CNT tag (id=1). qwc is auto-computed from payload.size(). vif_pad is
    // placed at bytes 12..15 of the tag (becomes cur.words[0] / cur.hdr[0]
    // for UNPACK).
    void cnt_tag(std::uint32_t vif_cmd, std::uint32_t vif_pad,
                 std::span<const std::uint8_t> payload) {
        const std::uint32_t qwc = static_cast<std::uint32_t>((payload.size() + 15u) / 16u);
        const std::uint64_t lo = (static_cast<std::uint64_t>(qwc) & 0xFFFFu) | (1ull << 28);
        put_u32(static_cast<std::uint32_t>(lo));
        put_u32(static_cast<std::uint32_t>(lo >> 32));
        put_u32(vif_cmd);
        put_u32(vif_pad);
        bytes.insert(bytes.end(), payload.begin(), payload.end());
        // Pad payload to qwc * 16 bytes (DMA tags are 16-byte aligned).
        for (std::size_t i = payload.size(); i < qwc * 16u; ++i) {
            bytes.push_back(0);
        }
    }

    void cnt_vif_only(std::uint32_t vif_cmd, std::uint32_t vif_pad = 0) {
        cnt_tag(vif_cmd, vif_pad, std::span<const std::uint8_t>{});
    }

    // END terminator (id=7). Carries a NOP VIF code; not strictly required for
    // the decoder to terminate (REFE id=0 / RET id=6 with empty stack also
    // work), but matches tools/model_dump.cpp output.
    void end_tag() {
        const std::uint64_t lo = (7ull << 28); // qwc=0, id=7
        put_u32(static_cast<std::uint32_t>(lo));
        put_u32(0);
        put_u32(0); // NOP VIF cmd (0x00)
        put_u32(0);
    }

    // RET terminator (id=6). The strict decoder's DMA walker treats RET with
    // an empty CALL stack as a clean terminator (matches the spec's
    // "CNT...RET" chain shape).
    void ret_tag() {
        const std::uint64_t lo = (6ull << 28); // qwc=0, id=6
        put_u32(static_cast<std::uint32_t>(lo));
        put_u32(0);
        put_u32(0); // NOP VIF cmd (0x00)
        put_u32(0);
    }

    // UNPACK V4-32 (vnvl=0xC), n=1, addr=0: a 4-word GIF tag (hdr[0..3]).
    // hdr0 is placed at the VIF pad position (becomes cur.hdr[0]); hdr1..3
    // become payload[0..2]. The 4th payload qword is unused by the decoder
    // and zero-padded by cnt_tag.
    void gif_tag_unpack(std::uint32_t hdr0, std::uint32_t hdr1,
                        std::uint32_t hdr2, std::uint32_t hdr3) {
        const std::uint32_t vif_cmd = (0x6Cu << 24) | (1u << 16) | 0u;
        std::vector<std::uint8_t> p;
        ::put_u32(p, hdr1);
        ::put_u32(p, hdr2);
        ::put_u32(p, hdr3);
        cnt_tag(vif_cmd, hdr0, p);
    }

    // UNPACK V3-32 (vnvl=0x8), n=n_vecs, addr=0: raw V3-32 vertex vectors.
    // The decoder reads n_vecs * 3 u32s starting at offset 4 (after the VIF
    // cmd). The first u32 comes from the VIF pad (chain bytes 12..15), the
    // remaining (n_vecs*3 - 1) u32s from the payload. Pass `all_words` with
    // size == n_vecs * 3 for full control; shorter vectors are zero-padded
    // up to n_vecs*3 (extra words become unused tail of cur.words).
    void v3_32_unpack(std::uint32_t n_vecs, std::span<const std::uint32_t> all_words,
                      std::uint16_t addr = 0) {
        const std::size_t want = static_cast<std::size_t>(n_vecs) * 3u;
        if (all_words.size() > want) {
            throw std::logic_error("v3_32_unpack: word count > n_vecs * 3");
        }
        const std::uint32_t vif_cmd =
            (0x68u << 24) | ((n_vecs & 0xFFu) << 16) | (addr & 0x3FFu);
        std::vector<std::uint32_t> padded_words(want, 0u);
        for (std::size_t i = 0; i < all_words.size(); ++i) {
            padded_words[i] = all_words[i];
        }
        // First u32 is the VIF pad; the rest are payload.
        std::vector<std::uint8_t> p;
        for (std::size_t i = 1; i < padded_words.size(); ++i) {
            ::put_u32(p, padded_words[i]);
        }
        cnt_tag(vif_cmd, padded_words[0], p);
    }

    // Convenience: a Material A+D block. The decoder only cares that vnvl=0x8
    // and addr=0x192; the actual vector data is read but discarded. We pass
    // a single zero vector (n=1) with 16 bytes of payload so the size check
    // `o + size > total` passes (size=12 for V3-32 with n=1).
    void material_ad_unpack() {
        const std::uint32_t n_vecs = 1;
        const std::uint32_t vif_cmd =
            (0x68u << 24) | ((n_vecs & 0xFFu) << 16) | 0x192u;
        std::vector<std::uint8_t> payload(16, 0);
        cnt_tag(vif_cmd, 0, payload);
    }

    // MSCAL: cmd=0x14, IMM = 2*entry (decoder computes entry = imm*8/16).
    void mscal(std::uint32_t entry) {
        const std::uint32_t imm = entry * 2u;
        const std::uint32_t vif_cmd = (0x14u << 24) | (0u << 16) | (imm & 0xFFFFu);
        cnt_vif_only(vif_cmd, 0);
    }

    // MSCALF: cmd=0x15, IMM = 2*entry.
    void mscalf(std::uint32_t entry) {
        const std::uint32_t imm = entry * 2u;
        const std::uint32_t vif_cmd = (0x15u << 24) | (0u << 16) | (imm & 0xFFFFu);
        cnt_vif_only(vif_cmd, 0);
    }

    // MSCNT: cmd=0x17. The strict decoder forces entry=-2 -> Unsupported ->
    // BadTag. We still pass no payload.
    void mscnt() {
        const std::uint32_t vif_cmd = (0x17u << 24);
        cnt_vif_only(vif_cmd, 0);
    }
};

// ---------------------------------------------------------------------------
// GIF tag value helpers. NLOOP lives in hdr[0] low 15 bits; PRIM in hdr[1]
// bits 15..17 (the low 3 bits of (hdr[1] >> 15)).
// ---------------------------------------------------------------------------
std::uint32_t gif_hdr0_nloop(std::uint32_t nloop, bool eop = true) {
    return (nloop & 0x7FFFu) | (eop ? (1u << 15) : 0u);
}
std::uint32_t gif_hdr1_prim(std::uint32_t prim_type) {
    // PRIM is bits 15..23 of hdr[1] in real GS chains; the decoder uses only
    // (hdr[1] >> 15) & 7. Set prim_type at bit 15.
    return (prim_type & 7u) << 15;
}

} // namespace

// ===========================================================================
// test_entry2_strip_4verts: the canonical happy path.
//   entry=2 (IMM=4) -> layout stride=4 (pos, norm, col, stq)
//   4 vertices; PRIM=0 (strip); colour clamp+round checks; alpha=255;
//   q-divide NOT applied for entry=2 even with nonzero q.
// ===========================================================================
namespace {

void test_entry2_strip_4verts() {
    ChainBuilder b;

    // GIF tag: NLOOP=4, PRIM=0 (strip).
    b.gif_tag_unpack(gif_hdr0_nloop(4), gif_hdr1_prim(0), 0u, 0u);

    // 4 vertices * 4 stride = 16 V3-32 vectors. all_words layout:
    //   [0..2] = pos for v0   [3..5] = norm  [6..8] = col  [9..11] = stq
    //   ... stride 4 ...
    //   ... and so on for v1..v3.
    // The VIF pad is words[0]; we set it to 0.0f so v0.px == 0.
    std::vector<std::uint32_t> words(48, 0u);
    auto Wf = [&](std::size_t i, float v) { words[i] = std::bit_cast<std::uint32_t>(v); };

    // Vertex 0: pos (px=0 from VIF pad), distinctive py/pz; full 4-byte colour.
    Wf(1, -1.5f);  Wf(2,  2.25f);   // py, pz
    Wf(3,  0.5f);  Wf(4,  0.25f); Wf(5,  0.875f);   // norm
    Wf(6, 100.0f); Wf(7, 150.0f); Wf(8, 200.0f);     // r, g, b
    Wf(9,  0.125f); Wf(10, 0.875f); Wf(11, 1.0f);    // u, v, q (nonzero -> not divided for entry=2)

    // Vertex 1: full position; clamp+round for colours.
    Wf(12, 11.0f); Wf(13, 22.0f); Wf(14, 33.0f);     // pos
    Wf(15, -0.5f); Wf(16, -0.25f); Wf(17, -0.875f);  // norm
    Wf(18, 255.0f); Wf(19, 254.5f); Wf(20, 0.5f);    // r, g, b (round-trip edge values)
    Wf(21, 0.0f); Wf(22, 1.0f); Wf(23, 0.0f);         // u=0, v=1, q=0

    // Vertex 2: out-of-range colours to exercise clamp + the !isfinite guard.
    Wf(24, 0.0f); Wf(25, 0.0f); Wf(26, 0.0f);
    Wf(27, 0.0f); Wf(28, 0.0f); Wf(29, 0.0f);
    Wf(30, 300.0f); Wf(31, -50.0f); Wf(32, std::numeric_limits<float>::infinity());
    Wf(33, 0.0f); Wf(34, 0.0f); Wf(35, 0.0f);

    // Vertex 3: NaN to exercise the !isfinite -> 0 path.
    Wf(36, 0.0f); Wf(37, 0.0f); Wf(38, 0.0f);
    Wf(39, 0.0f); Wf(40, 0.0f); Wf(41, 0.0f);
    Wf(42, std::numeric_limits<float>::quiet_NaN());
    Wf(43, std::numeric_limits<float>::quiet_NaN());
    Wf(44, std::numeric_limits<float>::quiet_NaN());
    Wf(45, 0.0f); Wf(46, 0.0f); Wf(47, 0.0f);

    b.v3_32_unpack(16, words);
    b.mscal(2); // entry=2 (IMM=4)
    b.end_tag();

    VifChainResult r = run_decode(b.bytes, "entry2 strip 4v");
    check(r.error == VifChainError::Code::Ok,
          std::string("entry2 strip 4v: expected Ok, got ") + err_name(r.error));
    if (r.error != VifChainError::Code::Ok) return;
    check(r.batches.size() == 1, "entry2 strip 4v: exactly 1 batch");
    if (r.batches.size() != 1) return;

    const DecodedBatch& batch = r.batches[0];
    check(batch.nloop == 4u, "batch.nloop == 4 from GIF hdr0");
    check(batch.prim == 0u, "batch.prim == 0 (strip)");
    check(batch.materialImage == 0xFFFFFFFFu,
          "no prior material A+D -> batch.materialImage is the unresolved sentinel");
    check(batch.vertices.size() == 4, "batch carries 4 vertices");
    if (batch.vertices.size() != 4) return;

    // Vertex 0: position, normal, colour, UV (q-divide skipped for entry=2).
    const Vertex& v0 = batch.vertices[0];
    check(v0.px == 0.0f && v0.py == -1.5f && v0.pz == 2.25f, "v0 position");
    check(v0.nx == 0.5f && v0.ny == 0.25f && v0.nz == 0.875f, "v0 normal");
    check(v0.r == 100 && v0.g == 150 && v0.b == 200, "v0 colour round-trips (in-range)");
    check(v0.a == 255, "v0 alpha is hardcoded to 255");
    check(v0.u == 0.125f && v0.v == 0.875f,
          "v0 UV preserved (entry=2 does NOT divide by q even when q!=0)");

    // Vertex 1: clamp/round edge values.
    const Vertex& v1 = batch.vertices[1];
    check(v1.px == 11.0f && v1.py == 22.0f && v1.pz == 33.0f, "v1 position");
    check(v1.nx == -0.5f && v1.ny == -0.25f && v1.nz == -0.875f, "v1 normal (signed)");
    check(v1.r == 255 && v1.g == 255 && v1.b == 1, "v1 colour clamp+round (255/254.5->255, 0.5->1)");
    check(v1.a == 255, "v1 alpha is 255");
    check(v1.u == 0.0f && v1.v == 1.0f, "v1 UV preserved (q=0 -> no divide)");

    // Vertex 2: clamp + the !isfinite guard.
    const Vertex& v2 = batch.vertices[2];
    check(v2.r == 255 && v2.g == 0 && v2.b == 0,
          "v2 colour clamp (300->255, -50->0, +inf->0 via !isfinite guard before clamp)");
    check(v2.a == 255, "v2 alpha is 255");

    // Vertex 3: NaN colours all map to 0.
    const Vertex& v3 = batch.vertices[3];
    check(v3.r == 0 && v3.g == 0 && v3.b == 0, "v3 NaN colours all map to 0");
    check(v3.a == 255, "v3 alpha is 255");

    // Strip indices: n=4 produces 2 triangles. i=0 (even): 0,1,2; i=1 (odd): 2,3,4
    // -> swap(b,c) -> 1,3,2.
    check(batch.indices.size() == 6, "strip 4v -> 2 triangles = 6 indices");
    check(batch.indices == std::vector<std::uint32_t>{0, 1, 2, 1, 3, 2},
          "strip indices: even-i {0,1,2}, odd-i {1,3,2} (b,c swapped)");
}

// ===========================================================================
// test_entry4_q_divide: entry=4 (IMM=8) layout. Per the decoder:
//   stride=5, first=0, posVec=0, colVec=1, stVec=4
//   q-divide IS applied for entry != 2 when q!=0 and finite.
// ===========================================================================
void test_entry4_q_divide() {
    ChainBuilder b;
    b.gif_tag_unpack(gif_hdr0_nloop(3), gif_hdr1_prim(0), 0u, 0u);

    // 3 vertices * 5 stride = 15 V3-32 vectors. words[0]=VIF pad (v0.px=0).
    std::vector<std::uint32_t> words(45, 0u);
    auto Wf = [&](std::size_t i, float v) { words[i] = std::bit_cast<std::uint32_t>(v); };

    // Vertex 0: pos (px=0 from VIF pad); q=2 with s=4 -> u=2, t=6->3
    Wf(1,  10.0f); Wf(2,  20.0f);                         // py, pz
    Wf(3,  10.0f); Wf(4,  20.0f); Wf(5,  30.0f);          // col
    // stride 5: vec 0,1,2,3,4. pos at vec 0 (words[0..2]), col at vec 1 (3..5),
    // vec 2,3 padding/unused, stq at vec 4 (12..14) for v0.
    Wf(12, 4.0f); Wf(13, 6.0f); Wf(14, 2.0f);              // s, t, q (nonzero, finite -> divides)

    // Vertex 1: q=0 -> no divide (s,t preserved). pos base = 5.
    Wf(15, 100.0f); Wf(16, 200.0f); Wf(18, 5.0f);          // py, pz, col0
    Wf(19, 6.0f); Wf(20, 7.0f);                            // col1, col2
    Wf(27, 7.5f); Wf(28, 8.5f); Wf(29, 0.0f);              // s, t, q=0

    // Vertex 2: q=NaN -> no divide. pos base = 10.
    Wf(30, 1000.0f); Wf(31, 2000.0f); Wf(33, 9.0f);        // py, pz, col0
    Wf(34, 10.0f); Wf(35, 11.0f);                          // col1, col2
    Wf(42, 1.5f); Wf(43, 2.5f);                            // s, t
    Wf(44, std::numeric_limits<float>::quiet_NaN());       // q=NaN

    b.v3_32_unpack(15, words);
    b.mscal(4); // entry=4 (IMM=8)
    b.end_tag();

    VifChainResult r = run_decode(b.bytes, "entry4 q-divide");
    check(r.error == VifChainError::Code::Ok,
          std::string("entry4 q-divide: expected Ok, got ") + err_name(r.error));
    if (r.error != VifChainError::Code::Ok) return;
    check(r.batches.size() == 1, "entry4 q-divide: exactly 1 batch");
    if (r.batches.size() != 1) return;

    const DecodedBatch& batch = r.batches[0];
    check(batch.vertices.size() == 3, "entry4: 3 vertices");
    if (batch.vertices.size() != 3) return;

    const Vertex& v0 = batch.vertices[0];
    check(v0.u == 2.0f && v0.v == 3.0f,
          "entry4 v0 q-divide applied (q=2, s=4/t=6 -> u=2/v=3)");
    const Vertex& v1 = batch.vertices[1];
    check(v1.u == 7.5f && v1.v == 8.5f,
          "entry4 v1 q=0 -> no divide (u=7.5, v=8.5 preserved)");
    const Vertex& v2 = batch.vertices[2];
    check(v2.u == 1.5f && v2.v == 2.5f,
          "entry4 v2 q=NaN -> no divide (u, v preserved; !isfinite guard)");
}

// ===========================================================================
// test_entry6_stride3: entry=6 (IMM=12) layout. stride=3 (pos, col, stq).
// No normals; q-divide applied (entry != 2).
// ===========================================================================
void test_entry6_stride3() {
    ChainBuilder b;
    b.gif_tag_unpack(gif_hdr0_nloop(3), gif_hdr1_prim(3), 0u, 0u); // PRIM=3 list

    // 3 vertices * 3 stride = 9 V3-32 vectors.
    std::vector<std::uint32_t> words(27, 0u);
    auto Wf = [&](std::size_t i, float v) { words[i] = std::bit_cast<std::uint32_t>(v); };

    // Vertex 0 (base 0): words[0..2]=pos, [3..5]=col, [6..8]=stq.
    Wf(1, 1.0f); Wf(2, 2.0f);
    Wf(3, 50.0f); Wf(4, 60.0f); Wf(5, 70.0f);
    Wf(6, 8.0f); Wf(7, 16.0f); Wf(8, 2.0f); // q=2 -> divide

    // Vertex 1 (base 3): words[9..11], [12..14], [15..17]
    Wf(10, 3.0f); Wf(11, 4.0f);
    Wf(12, 80.0f); Wf(13, 90.0f); Wf(14, 100.0f);
    Wf(15, 4.0f); Wf(16, 8.0f); Wf(17, 2.0f); // divide

    // Vertex 2 (base 6): words[18..20], [21..23], [24..26]
    Wf(19, 5.0f); Wf(20, 6.0f);
    Wf(21, 110.0f); Wf(22, 120.0f); Wf(23, 130.0f);
    Wf(24, 6.0f); Wf(25, 12.0f); Wf(26, 2.0f); // divide

    b.v3_32_unpack(9, words);
    b.mscal(6); // entry=6 (IMM=12)
    b.end_tag();

    VifChainResult r = run_decode(b.bytes, "entry6 stride3");
    check(r.error == VifChainError::Code::Ok,
          std::string("entry6 stride3: expected Ok, got ") + err_name(r.error));
    if (r.error != VifChainError::Code::Ok) return;
    check(r.batches.size() == 1, "entry6: 1 batch");
    if (r.batches.size() != 1) return;

    const DecodedBatch& batch = r.batches[0];
    check(batch.vertices.size() == 3, "entry6: 3 vertices");
    check(batch.prim == 3u, "entry6: PRIM=3 (list)");

    const Vertex& v0 = batch.vertices[0];
    check(v0.px == 0.0f && v0.py == 1.0f && v0.pz == 2.0f, "v0 pos (px from VIF pad)");
    check(v0.r == 50 && v0.g == 60 && v0.b == 70, "v0 colour");
    check(v0.u == 4.0f && v0.v == 8.0f, "v0 q-divide (8/2=4, 16/2=8)");
    const Vertex& v1 = batch.vertices[1];
    check(v1.u == 2.0f && v1.v == 4.0f, "v1 q-divide (4/2=2, 8/2=4)");
    const Vertex& v2 = batch.vertices[2];
    check(v2.u == 3.0f && v2.v == 6.0f, "v2 q-divide (6/2=3, 12/2=6)");

    // PRIM=3 (list) with n=3 -> 1 triangle: {0,1,2}.
    check(batch.indices.size() == 3, "PRIM=3 list n=3 -> 3 indices");
    check(batch.indices == std::vector<std::uint32_t>{0, 1, 2}, "PRIM=3 list indices");
}

// ===========================================================================
// test_entry3_centre_offset: entry=3 (IMM=6) layout. stride=5, first=1.
// The first vector is the centre; per-vertex offsets are added to it.
// ===========================================================================
void test_entry3_centre_offset() {
    ChainBuilder b;
    b.gif_tag_unpack(gif_hdr0_nloop(2), gif_hdr1_prim(0), 0u, 0u);

    // 2 vertices * 5 stride + 1 centre = 11 vectors = 33 words.
    // words[0..2] = centre (words[0] is VIF pad; we choose centre.px = 100.0f).
    std::vector<std::uint32_t> words(33, 0u);
    auto Wf = [&](std::size_t i, float v) { words[i] = std::bit_cast<std::uint32_t>(v); };

    // Centre at words[0..2]: px = VIF pad value, py = words[1], pz = words[2].
    Wf(1, 30.0f); Wf(2, 50.0f);
    const std::uint32_t centre_px_bits = std::bit_cast<std::uint32_t>(100.0f);
    // (VIF pad goes into words[0] via v3_32_unpack; we override it below.)

    // Vertex 0: base = 1 + 0*5 = 1. pos at vec 1 (words[3..5]), col at vec 2
    // (words[6..8]), stq at vec 5 (words[15..17]).
    Wf(3, 1.0f); Wf(4, 2.0f); Wf(5, 3.0f);                // pos offset
    Wf(6, 10.0f); Wf(7, 20.0f); Wf(8, 30.0f);             // col
    // vec 3 (words[9..11]) and vec 4 (words[12..14]) unused.
    Wf(15, 0.5f); Wf(16, 0.5f); Wf(17, 0.0f);            // stq, q=0 (entry=3 -> divide if q!=0)

    // Vertex 1: base = 1 + 1*5 = 6. pos at vec 6 (words[18..20]), col at vec 7
    // (words[21..23]), stq at vec 10 (words[30..32]).
    Wf(18, -5.0f); Wf(19, 7.0f); Wf(20, -11.0f);          // pos offset
    Wf(21, 40.0f); Wf(22, 50.0f); Wf(23, 60.0f);          // col
    Wf(30, 4.0f); Wf(31, 8.0f); Wf(32, 2.0f);            // stq with q=2 -> divide

    // Inject the centre.px into the VIF pad by emitting the words manually.
    std::vector<std::uint8_t> payload;
    for (std::size_t i = 1; i < words.size(); ++i) put_u32(payload, words[i]);
    const std::uint32_t vif_cmd = (0x68u << 24) | (11u << 16) | 0u;
    b.cnt_tag(vif_cmd, centre_px_bits, payload);

    b.mscal(3); // entry=3 (IMM=6)
    b.end_tag();

    VifChainResult r = run_decode(b.bytes, "entry3 centre");
    check(r.error == VifChainError::Code::Ok,
          std::string("entry3 centre: expected Ok, got ") + err_name(r.error));
    if (r.error != VifChainError::Code::Ok) return;
    check(r.batches.size() == 1, "entry3 centre: 1 batch");
    if (r.batches.size() != 1) return;

    const DecodedBatch& batch = r.batches[0];
    check(batch.vertices.size() == 2, "entry3 centre: 2 vertices");
    if (batch.vertices.size() != 2) return;

    // Vertex 0: pos = centre + offset = (100+1, 30+2, 50+3) = (101, 32, 53).
    const Vertex& v0 = batch.vertices[0];
    check(v0.px == 101.0f && v0.py == 32.0f && v0.pz == 53.0f,
          "v0 position = centre + offset ((100+1, 30+2, 50+3))");
    check(v0.u == 0.5f && v0.v == 0.5f, "v0 UV preserved (q=0)");

    // Vertex 1: pos = (100-5, 30+7, 50-11) = (95, 37, 39); q=2 -> u=2, v=4.
    const Vertex& v1 = batch.vertices[1];
    check(v1.px == 95.0f && v1.py == 37.0f && v1.pz == 39.0f,
          "v1 position = centre + offset ((100-5, 30+7, 50-11))");
    check(v1.u == 2.0f && v1.v == 4.0f, "v1 q-divide applied (q=2)");
}

// ===========================================================================
// test_prim_list_fan_strip: explicit index-order checks for PRIM=3, PRIM=5,
// PRIM=0 with enough vertices to make every branch distinct.
// ===========================================================================
void test_prim_list_fan_strip() {
    // (a) PRIM=3 (list) with n=6 -> {0,1,2} and {3,4,5}.
    {
        ChainBuilder b;
        b.gif_tag_unpack(gif_hdr0_nloop(6), gif_hdr1_prim(3), 0u, 0u);
        // 6 vertices * 3 stride (entry=6 layout for simplicity) = 18 vectors.
        std::vector<std::uint32_t> words(18, 0u);
        b.v3_32_unpack(18, words);
        b.mscal(6);
        b.end_tag();

        VifChainResult r = run_decode(b.bytes, "prim list");
        check(r.error == VifChainError::Code::Ok,
              std::string("prim list: expected Ok, got ") + err_name(r.error));
        if (r.error == VifChainError::Code::Ok && !r.batches.empty()) {
            const auto& idx = r.batches[0].indices;
            check(idx == std::vector<std::uint32_t>{0, 1, 2, 3, 4, 5},
                  "PRIM=3 list n=6 -> indices {0,1,2,3,4,5}");
        }
    }

    // (b) PRIM=5 (fan) with n=5 -> {0,1,2}, {0,2,3}, {0,3,4}.
    {
        ChainBuilder b;
        b.gif_tag_unpack(gif_hdr0_nloop(5), gif_hdr1_prim(5), 0u, 0u);
        std::vector<std::uint32_t> words(15, 0u); // 5 v * 3 stride
        b.v3_32_unpack(15, words);
        b.mscal(6);
        b.end_tag();

        VifChainResult r = run_decode(b.bytes, "prim fan");
        check(r.error == VifChainError::Code::Ok,
              std::string("prim fan: expected Ok, got ") + err_name(r.error));
        if (r.error == VifChainError::Code::Ok && !r.batches.empty()) {
            const auto& idx = r.batches[0].indices;
            check(idx == std::vector<std::uint32_t>{0, 1, 2, 0, 2, 3, 0, 3, 4},
                  "PRIM=5 fan n=5 -> {0,1,2}, {0,2,3}, {0,3,4}");
        }
    }

    // (c) PRIM=0 (strip) with n=5 -> {0,1,2} {1,3,2} {2,3,4} (i=1 swaps b,c).
    {
        ChainBuilder b;
        b.gif_tag_unpack(gif_hdr0_nloop(5), gif_hdr1_prim(0), 0u, 0u);
        std::vector<std::uint32_t> words(15, 0u);
        b.v3_32_unpack(15, words);
        b.mscal(6);
        b.end_tag();

        VifChainResult r = run_decode(b.bytes, "prim strip");
        check(r.error == VifChainError::Code::Ok,
              std::string("prim strip: expected Ok, got ") + err_name(r.error));
        if (r.error == VifChainError::Code::Ok && !r.batches.empty()) {
            const auto& idx = r.batches[0].indices;
            check(idx == std::vector<std::uint32_t>{0, 1, 2, 1, 3, 2, 2, 3, 4},
                  "PRIM=0 strip n=5 -> even-i {0,1,2}, odd-i {1,3,2}, {2,3,4}");
        }
    }

    // (d) Strip with PRIM=1 (type 1 falls into the default branch, treated as
    // strip): same odd-i swap rule.
    {
        ChainBuilder b;
        b.gif_tag_unpack(gif_hdr0_nloop(4), gif_hdr1_prim(1), 0u, 0u);
        std::vector<std::uint32_t> words(12, 0u); // 4 v * 3 stride
        b.v3_32_unpack(12, words);
        b.mscal(6);
        b.end_tag();

        VifChainResult r = run_decode(b.bytes, "prim type1");
        check(r.error == VifChainError::Code::Ok,
              std::string("prim type1: expected Ok, got ") + err_name(r.error));
        if (r.error == VifChainError::Code::Ok && !r.batches.empty()) {
            const auto& idx = r.batches[0].indices;
            check(idx == std::vector<std::uint32_t>{0, 1, 2, 1, 3, 2},
                  "PRIM type 1 (default branch / strip) n=4 -> {0,1,2} {1,3,2}");
        }
    }

    // (e) NLOOP < 3 -> buildIndices returns no indices (prim=strip default).
    {
        ChainBuilder b;
        b.gif_tag_unpack(gif_hdr0_nloop(2), gif_hdr1_prim(0), 0u, 0u);
        std::vector<std::uint32_t> words(6, 0u);
        b.v3_32_unpack(6, words);
        b.mscal(6);
        b.end_tag();

        VifChainResult r = run_decode(b.bytes, "prim short");
        check(r.error == VifChainError::Code::Ok,
              std::string("prim short: expected Ok, got ") + err_name(r.error));
        if (r.error == VifChainError::Code::Ok && !r.batches.empty()) {
            check(r.batches[0].vertices.size() == 2,
                  "prim short: NLOOP=2 -> 2 vertices even with no triangles");
            check(r.batches[0].indices.empty(),
                  "prim short: NLOOP<3 -> no indices (buildIndices early-out)");
        }
    }
}

// ===========================================================================
// test_material_ad_sticky: one material A+D UNPACK increments both binders'
// internal counter; subsequent MSCAL batches get materialImage = 0 (the
// sticky counter value). A second A+D increments to 1, and a chain without
// any A+D keeps the unresolved sentinel.
// ===========================================================================
void test_material_ad_sticky() {
    // (a) No material -> both batches carry the unresolved sentinel.
    {
        ChainBuilder b;
        b.gif_tag_unpack(gif_hdr0_nloop(1), gif_hdr1_prim(0), 0u, 0u);
        std::vector<std::uint32_t> words1(3, 0u);
        b.v3_32_unpack(3, words1);
        b.mscal(6);
        b.gif_tag_unpack(gif_hdr0_nloop(1), gif_hdr1_prim(0), 0u, 0u);
        std::vector<std::uint32_t> words2(3, 0u);
        b.v3_32_unpack(3, words2);
        b.mscal(6);
        b.end_tag();

        VifChainResult r = run_decode(b.bytes, "no material");
        check(r.error == VifChainError::Code::Ok,
              std::string("no material: expected Ok, got ") + err_name(r.error));
        if (r.error == VifChainError::Code::Ok && r.batches.size() == 2) {
            check(r.batches[0].materialImage == 0xFFFFFFFFu,
                  "no material -> batch[0].materialImage sentinel");
            check(r.batches[1].materialImage == 0xFFFFFFFFu,
                  "no material -> batch[1].materialImage sentinel");
        } else {
            check(false, "no material: 2 batches decoded");
        }
    }

    // (b) One material A+D -> both batches carry materialImage = 0 (sticky).
    {
        ChainBuilder b;
        b.material_ad_unpack(); // increments material to 0
        b.gif_tag_unpack(gif_hdr0_nloop(1), gif_hdr1_prim(0), 0u, 0u);
        std::vector<std::uint32_t> words1(3, 0u);
        b.v3_32_unpack(3, words1);
        b.mscal(6);
        b.gif_tag_unpack(gif_hdr0_nloop(1), gif_hdr1_prim(0), 0u, 0u);
        std::vector<std::uint32_t> words2(3, 0u);
        b.v3_32_unpack(3, words2);
        b.mscal(6);
        b.end_tag();

        VifChainResult r = run_decode(b.bytes, "sticky material");
        check(r.error == VifChainError::Code::Ok,
              std::string("sticky material: expected Ok, got ") + err_name(r.error));
        if (r.error == VifChainError::Code::Ok && r.batches.size() == 2) {
            check(r.batches[0].materialImage == 0u,
                  "sticky: batch[0].materialImage == 0 after one A+D");
            check(r.batches[1].materialImage == 0u,
                  "sticky: batch[1].materialImage stays 0 across MSCALs");
        } else {
            check(false, "sticky material: 2 batches decoded");
        }
    }

    // (c) Two A+D blocks -> batches after the second carry materialImage = 1.
    {
        ChainBuilder b;
        b.material_ad_unpack(); // material -> 0
        b.gif_tag_unpack(gif_hdr0_nloop(1), gif_hdr1_prim(0), 0u, 0u);
        std::vector<std::uint32_t> words1(3, 0u);
        b.v3_32_unpack(3, words1);
        b.mscal(6); // materialImage = 0
        b.material_ad_unpack(); // material -> 1
        b.gif_tag_unpack(gif_hdr0_nloop(1), gif_hdr1_prim(0), 0u, 0u);
        std::vector<std::uint32_t> words2(3, 0u);
        b.v3_32_unpack(3, words2);
        b.mscal(6); // materialImage = 1
        b.end_tag();

        VifChainResult r = run_decode(b.bytes, "two materials");
        check(r.error == VifChainError::Code::Ok,
              std::string("two materials: expected Ok, got ") + err_name(r.error));
        if (r.error == VifChainError::Code::Ok && r.batches.size() == 2) {
            check(r.batches[0].materialImage == 0u,
                  "two materials: batch[0] sees materialImage = 0 (before 2nd A+D)");
            check(r.batches[1].materialImage == 1u,
                  "two materials: batch[1] sees materialImage = 1 (sticky after 2nd A+D)");
        } else {
            check(false, "two materials: 2 batches decoded");
        }
    }
}

// ===========================================================================
// test_mscal_mscalf_mscnt_batch_close: MSCAL/MSCALF close a batch and reset
// cur; MSCNT maps to entry=-2 -> no layout -> BadTag (the strict decoder
// diverges from the lenient tools/model_dump.cpp here on purpose).
// ===========================================================================
void test_mscal_mscalf_mscnt_batch_close() {
    // (a) MSCAL closes a valid batch.
    {
        ChainBuilder b;
        b.gif_tag_unpack(gif_hdr0_nloop(2), gif_hdr1_prim(0), 0u, 0u);
        std::vector<std::uint32_t> words(6, 0u); // 2 v * 3 stride
        b.v3_32_unpack(6, words);
        b.mscal(6);
        b.end_tag();

        VifChainResult r = run_decode(b.bytes, "mscal close");
        check(r.error == VifChainError::Code::Ok,
              std::string("mscal close: expected Ok, got ") + err_name(r.error));
        check(r.batches.size() == 1, "mscal close: exactly 1 batch emitted");
    }

    // (b) MSCALF closes a valid batch.
    {
        ChainBuilder b;
        b.gif_tag_unpack(gif_hdr0_nloop(2), gif_hdr1_prim(0), 0u, 0u);
        std::vector<std::uint32_t> words(6, 0u);
        b.v3_32_unpack(6, words);
        b.mscalf(6);
        b.end_tag();

        VifChainResult r = run_decode(b.bytes, "mscalf close");
        check(r.error == VifChainError::Code::Ok,
              std::string("mscalf close: expected Ok, got ") + err_name(r.error));
        check(r.batches.size() == 1, "mscalf close: exactly 1 batch emitted");
    }

    // (c) MSCNT (cmd=0x17) -> entry=-2 -> Unsupported -> BadTag.
    {
        ChainBuilder b;
        b.gif_tag_unpack(gif_hdr0_nloop(2), gif_hdr1_prim(0), 0u, 0u);
        std::vector<std::uint32_t> words(6, 0u);
        b.v3_32_unpack(6, words);
        b.mscnt();
        b.end_tag();

        expect_err(b.bytes, VifChainError::Code::BadTag, "mscnt -> BadTag (entry=-2)");
    }

    // (d) MSCAL without a prior GIF tag UNPACK -> !cur.hasHdr -> BadTag.
    {
        ChainBuilder b;
        // No UNPACKs at all: cur.hasHdr is false on the first MSCAL.
        b.mscal(2);
        b.end_tag();

        expect_err(b.bytes, VifChainError::Code::BadTag,
                   "mscal without prior GIF tag -> BadTag");
    }

    // (e) MSCAL with an entry the layout doesn't recognise (entry=8 ->
    // IMM=0x10). buildVertices returns Unsupported -> BadTag.
    {
        ChainBuilder b;
        b.gif_tag_unpack(gif_hdr0_nloop(1), gif_hdr1_prim(0), 0u, 0u);
        std::vector<std::uint32_t> words(4, 0u); // some data, layout doesn't matter
        b.v3_32_unpack(4, words);
        b.mscal(8); // entry=8 (IMM=0x10) not in {2,3,4,5,6}
        b.end_tag();

        expect_err(b.bytes, VifChainError::Code::BadTag,
                   "mscal entry=8 (no layout) -> BadTag");
    }
}

// ===========================================================================
// test_multiple_batches: two batches in a single chain. Verifies that the
// decoder resets BatchState between MSCALs and accumulates 2 batches.
// ===========================================================================
void test_multiple_batches() {
    ChainBuilder b;

    // Batch 1: 2 vertices, PRIM=3 (list).
    b.gif_tag_unpack(gif_hdr0_nloop(2), gif_hdr1_prim(3), 0u, 0u);
    std::vector<std::uint32_t> w1(6, 0u);
    b.v3_32_unpack(6, w1);
    b.mscal(6);

    // Batch 2: 4 vertices, PRIM=0 (strip).
    b.gif_tag_unpack(gif_hdr0_nloop(4), gif_hdr1_prim(0), 0u, 0u);
    std::vector<std::uint32_t> w2(12, 0u);
    b.v3_32_unpack(12, w2);
    b.mscal(6);

    b.end_tag();

    VifChainResult r = run_decode(b.bytes, "multi batch");
    check(r.error == VifChainError::Code::Ok,
          std::string("multi batch: expected Ok, got ") + err_name(r.error));
    if (r.error != VifChainError::Code::Ok) return;
    check(r.batches.size() == 2, "multi batch: 2 batches accumulated");
    if (r.batches.size() != 2) return;
    check(r.batches[0].prim == 3u && r.batches[0].nloop == 2u && r.batches[0].vertices.size() == 2,
          "multi batch[0]: PRIM=3, nloop=2, 2 vertices");
    check(r.batches[1].prim == 0u && r.batches[1].nloop == 4u && r.batches[1].vertices.size() == 4,
          "multi batch[1]: PRIM=0, nloop=4, 4 vertices (BatchState reset between MSCALs)");
}

// ===========================================================================
// test_nloop_zero: NLOOP=0 -> batch closes with zero vertices and zero
// indices; buildVertices still records the prim but skips the loop.
// ===========================================================================
void test_nloop_zero() {
    ChainBuilder b;
    b.gif_tag_unpack(gif_hdr0_nloop(0), gif_hdr1_prim(0), 0u, 0u);
    const std::vector<std::uint32_t> zero(3, 0u); // some data; NLOOP=0 skips it
    b.v3_32_unpack(3, zero);
    b.mscal(6);
    b.end_tag();

    VifChainResult r = run_decode(b.bytes, "nloop zero");
    check(r.error == VifChainError::Code::Ok,
          std::string("nloop zero: expected Ok, got ") + err_name(r.error));
    if (r.error == VifChainError::Code::Ok && !r.batches.empty()) {
        check(r.batches[0].nloop == 0u, "nloop zero: batch.nloop == 0");
        check(r.batches[0].vertices.empty(), "nloop zero: no vertices emitted");
        check(r.batches[0].indices.empty(), "nloop zero: no indices emitted");
    } else {
        check(false, "nloop zero: 1 batch emitted");
    }
}

// ===========================================================================
// test_truncated_oversized_hostile: three distinct "input was bad" scenarios.
// All three must reject with a structured code and no partial batches.
// ===========================================================================
void test_truncated_oversized_hostile() {
    // (a) DMA tag header truncated: only 8 bytes, but the walker demands 16
    // bytes for the tag header. Truncated.
    {
        ChainBuilder b;
        // Append only 8 bytes — a partial tag.
        b.put_u32(0u); b.put_u32(0u);
        expect_err(b.bytes, VifChainError::Code::Truncated,
                   "DMA tag header truncated to 8 bytes");
    }

    // (b) DMA tag claims qwc that exceeds the remaining chain bytes. Truncated
    // when the walker tries to append the payload.
    {
        ChainBuilder b;
        // Construct a single CNT tag with a huge qwc but truncate the chain.
        const std::uint64_t lo = (4096u & 0xFFFFu) | (1ull << 28);
        b.put_u32(static_cast<std::uint32_t>(lo));
        b.put_u32(static_cast<std::uint32_t>(lo >> 32));
        // No VIF qword, no payload — tag is incomplete.
        expect_err(b.bytes, VifChainError::Code::Truncated,
                   "CNT qwc=4096 but no VIF qword or payload");
    }

    // (c) UNPACK claims n=200 vectors (2400 bytes) but the payload only holds
    // ~64 bytes (4 vectors). The strict decoder's `o + size > total` check
    // returns Truncated.
    {
        ChainBuilder b;
        b.gif_tag_unpack(gif_hdr0_nloop(1), gif_hdr1_prim(0), 0u, 0u);
        // VIF cmd for V3-32 with n=200 but only provide 12 bytes (3 floats).
        const std::uint32_t vif_cmd = (0x68u << 24) | (200u << 16) | 0u;
        const std::uint8_t payload[12] = {0};
        b.cnt_tag(vif_cmd, 0, payload);
        b.end_tag();

        expect_err(b.bytes, VifChainError::Code::Truncated,
                   "UNPACK V3-32 n=200 but only 12 bytes of payload");
    }

    // (d) Hostile NLOOP: the GIF tag NLOOP claims 100 vertices but the vertex
    // block only carries 16 vectors (= 4 vertices for entry=2). buildVertices
    // returns Overflow and the decoder surfaces Overflow with no partial
    // batches. We also include a first, GOOD batch before this hostile one to
    // verify that the previously emitted batch is dropped too.
    {
        ChainBuilder b;
        // Batch 1: 2 vertices, valid.
        b.gif_tag_unpack(gif_hdr0_nloop(2), gif_hdr1_prim(0), 0u, 0u);
        std::vector<std::uint32_t> good(8, 0u); // 2 v * 4 stride (entry=2 layout)
        b.v3_32_unpack(8, good);
        b.mscal(2);

        // Batch 2: hostile NLOOP=100 against only 16 vectors (4-vertex entry=2).
        b.gif_tag_unpack(gif_hdr0_nloop(100), gif_hdr1_prim(0), 0u, 0u);
        std::vector<std::uint32_t> bad(48, 0u); // 16 vectors * 3 words
        b.v3_32_unpack(16, bad);
        b.mscal(2);

        b.end_tag();

        VifChainResult r = run_decode(b.bytes, "hostile nloop");
        check(r.error == VifChainError::Code::Overflow,
              std::string("hostile nloop: expected Overflow, got ") + err_name(r.error));
        check(r.batches.empty(),
              "hostile nloop: previously-good batch[0] is also dropped (no partial batches)");
    }

    // (e) Hostile NLOOP, single hostile batch (no preceding good batch).
    {
        ChainBuilder b;
        b.gif_tag_unpack(gif_hdr0_nloop(100), gif_hdr1_prim(0), 0u, 0u);
        std::vector<std::uint32_t> bad(48, 0u); // 16 vectors
        b.v3_32_unpack(16, bad);
        b.mscal(2);
        b.end_tag();

        expect_err(b.bytes, VifChainError::Code::Overflow,
                   "single hostile NLOOP -> Overflow");
    }
}

// ===========================================================================
// test_unknown_vif_code: VIF code outside the recognised families. BadTag.
// ===========================================================================
void test_unknown_vif_code() {
    ChainBuilder b;
    // VIF cmd 0x09 is in the low-byte range but NOT in the explicit NOP set
    // {0x00..0x07} and is not 0x10/0x11/0x13. cmd < 0x60 so it skips the
    // UNPACK branch and falls through the switch into the default -> BadTag.
    b.cnt_vif_only(0x09000000u);
    b.end_tag();

    expect_err(b.bytes, VifChainError::Code::BadTag,
               "unknown VIF code 0x09 -> BadTag");
}

// ===========================================================================
// test_gif_tag_at_addr_zero_consumes_first_vector (sky/ELF path):
// When a V3-32 UNPACK arrives at addr=0 and no GIF tag has been seen yet, the
// first vector of the block IS the GIF tag. The decoder pulls hdr[0..2] from
// words[0..2] and shrinks the words vector by one.
// ===========================================================================
void test_sky_chain_in_vector_block() {
    ChainBuilder b;

    // We need 1 vertex with entry=6 layout -> stride=3 -> 3 vectors, plus 1
    // extra vector at the front that is treated as the GIF tag.
    // Total: 4 V3-32 vectors. words[0..2] = GIF tag; words[3..5] = vertex 0.
    std::vector<std::uint32_t> words(12, 0u);
    auto Wf = [&](std::size_t i, float v) { words[i] = std::bit_cast<std::uint32_t>(v); };

    // GIF tag: NLOOP=1, PRIM=0 (strip). hdr[0..2] -> words[0..2].
    const std::uint32_t hdr0 = (1u & 0x7FFFu) | (1u << 15);
    const std::uint32_t hdr1 = 0u; // PRIM=0 strip
    const std::uint32_t hdr2 = 0u;
    words[0] = hdr0;
    words[1] = hdr1;
    words[2] = hdr2;

    // Vertex 0 (after GIF tag removal): base=0 in entry=6 layout, stride=3.
    // pos at vec 0 (words[3..5]), col at vec 1 (words[6..8]), stq at vec 2
    // (words[9..11]).
    Wf(4, 5.0f); Wf(5, 6.0f);                              // py, pz (px = VIF pad = 0)
    Wf(6, 100.0f); Wf(7, 150.0f); Wf(8, 200.0f);           // col
    Wf(9, 0.25f); Wf(10, 0.75f); Wf(11, 0.0f);             // stq, q=0

    // addr=0 so the first vector becomes the GIF tag.
    b.v3_32_unpack(4, words, /*addr=*/0);
    b.mscal(6);
    b.end_tag();

    VifChainResult r = run_decode(b.bytes, "sky chain");
    check(r.error == VifChainError::Code::Ok,
          std::string("sky chain: expected Ok, got ") + err_name(r.error));
    if (r.error != VifChainError::Code::Ok || r.batches.empty()) return;
    check(r.batches[0].nloop == 1u, "sky chain: NLOOP=1 from the embedded GIF tag");
    check(r.batches[0].vertices.size() == 1, "sky chain: 1 vertex emitted");
    if (!r.batches[0].vertices.empty()) {
        const Vertex& v0 = r.batches[0].vertices[0];
        check(v0.px == 0.0f && v0.py == 5.0f && v0.pz == 6.0f, "sky chain v0 position");
        check(v0.r == 100 && v0.g == 150 && v0.b == 200, "sky chain v0 colour");
    }
}

// ===========================================================================
// test_color_edges: focused clamp+round edge cases via vertex colour bytes.
// (Distinct from the per-vertex checks; this isolates floatToColour's rules.)
// ===========================================================================
void test_color_edges() {
    // One vertex, entry=6 (stride=3). Words: pos (px=VIF pad=0, py=0, pz=0),
    // col (r, g, b), stq (u, v, q).
    std::vector<std::uint32_t> words(9, 0u);
    auto Wf = [&](std::size_t i, float v) { words[i] = std::bit_cast<std::uint32_t>(v); };
    Wf(3, 255.6f);   // r = 255.6 -> clamp 255, +0.5 = 255.5 -> floor 255 -> 255
    Wf(4, 254.5f);   // g = 254.5 -> 255.0 -> floor 255 -> 255
    Wf(5, 0.5f);     // b = 0.5 -> 1.0 -> floor 1 -> 1
    Wf(6, 0.0f); Wf(7, 0.0f); Wf(8, 0.0f);

    ChainBuilder b;
    b.gif_tag_unpack(gif_hdr0_nloop(1), gif_hdr1_prim(0), 0u, 0u);
    b.v3_32_unpack(3, words);
    b.mscal(6);
    b.end_tag();

    VifChainResult r = run_decode(b.bytes, "color edges");
    check(r.error == VifChainError::Code::Ok,
          std::string("color edges: expected Ok, got ") + err_name(r.error));
    if (r.error == VifChainError::Code::Ok && !r.batches.empty() &&
        !r.batches[0].vertices.empty()) {
        const Vertex& v = r.batches[0].vertices[0];
        check(v.r == 255 && v.g == 255 && v.b == 1,
              "255.6 clamp+round -> 255, 254.5 -> 255, 0.5 -> 1");
    }
}

// ===========================================================================
// test_ret_terminator: a CNT...RET chain (no END) with a valid V3-32 vertex
// block. RET (id=6) with an empty CALL stack is treated as a clean terminator
// by the DMA walker.
// ===========================================================================
void test_ret_terminator() {
    ChainBuilder b;
    b.gif_tag_unpack(gif_hdr0_nloop(2), gif_hdr1_prim(3), 0u, 0u);
    std::vector<std::uint32_t> words(18, 0u); // 2 v * 3 stride -> padded to 18
    b.v3_32_unpack(6, words);
    b.mscal(6);
    b.ret_tag(); // RET (id=6, qwc=0)

    VifChainResult r = run_decode(b.bytes, "ret terminator");
    check(r.error == VifChainError::Code::Ok,
          std::string("ret terminator: expected Ok, got ") + err_name(r.error));
    check(r.batches.size() == 1, "ret terminator: 1 batch emitted");
}

} // namespace

int main() {
    test_entry2_strip_4verts();
    test_entry4_q_divide();
    test_entry6_stride3();
    test_entry3_centre_offset();
    test_prim_list_fan_strip();
    test_material_ad_sticky();
    test_mscal_mscalf_mscnt_batch_close();
    test_multiple_batches();
    test_nloop_zero();
    test_truncated_oversized_hostile();
    test_unknown_vif_code();
    test_sky_chain_in_vector_block();
    test_color_edges();
    test_ret_terminator();

    if (g_failures > 0) {
        std::cerr << "test_vif_chain: " << g_failures << " of " << g_checks << " checks FAILED\n";
        return EXIT_FAILURE;
    }
    std::cout << "test_vif_chain: " << g_checks << " checks passed\n";
    return EXIT_SUCCESS;
}