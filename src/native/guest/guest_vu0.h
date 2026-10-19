#pragma once
// VU0 macro-instruction model for transliterated guest TUs (W11 VU0 package).
// Semantics mirror the oracle runtime exactly (ps2_vu_div etc. in
// ps2_runtime_macros.h and the SSE formulas in ps2xRecomp's
// vu_translation_helpers.cpp) so state hashes stay bit-identical.
// Float ops run on the game thread and inherit the thread's EE float mode
// (RC+FTZ+DAZ set once by the runtime) exactly as the oracle's SSE ops do.
//
// Model notes (oracle quirks we must match, not real-hardware behavior):
//  - vi[]: one uint16 per register (oracle models vi as 16 x uint16).
//  - vf0 initialized to (0,0,0,1); no write guard (oracle has none; game
//    never writes vf0).
//  - vsubq: fd = fs - Q (oracle formula; slaspec's extra vufd arg is inert).
//  - max/min: NaN returns the SECOND operand (SSE MAXPS/MINPS behavior).
//  - vsqi/vsqd field divergence (flag for W13 parity probe): the oracle's
//    translateVU_VSQI stores vf[rt-field] at vi[rd-field]; the Ghidra add-on
//    (and every coherent game use, e.g. vu0StoreMatrixQuadsAt4 0x21d3b8) has
//    data in the fs field and the vi index in the ft field. We follow the
//    add-on; the oracle reading stores one register three times, so its
//    decode of these sites is almost certainly never exercised by the
//    goldens. If W13 shows otherwise, mirror the oracle and note it here.
#include <cmath>
#include <cstdint>
#include <cstring>

#include "guest_shim.h"

namespace rtguest::vu0 {

struct Vu4 {
    float v[4];  // x, y, z, w
};

struct State {
    Vu4 vf[32]{};
    uint16_t vi[16]{};
    Vu4 acc{};
    float q = 0.f, i_ = 0.f, p = 0.f;
    uint32_t status = 0, mac = 0, clip = 0, r = 0, tpc = 0, cmsar0 = 0,
             fbrst = 0, vpu_stat = 0, cmsar1 = 0;
};

inline State S = [] {
    State s;
    s.vf[0] = {{0.f, 0.f, 0.f, 1.f}};
    return s;
}();

// dest-mask bit order (slaspec): bit3=.x(bit0 lane), bit2=.y, bit1=.z, bit0=.w
inline Vu4 blend(Vu4 d, Vu4 r, uint32_t dest) {
    for (int l = 0; l < 4; ++l)
        if (dest & (8u >> l)) d.v[l] = r.v[l];
    return d;
}
// Oracle: ps2_runtime_macros.h:959-988. FMAC results replace MAC and merge STATUS sticky bits.
inline void fmacFlags(Vu4 r, uint32_t dest) {
    uint32_t mac = 0;
    for (uint32_t lane = 0; lane != 4; ++lane) {
        const uint32_t bit = 3u - lane;
        if (!(dest & (1u << bit))) continue;
        uint32_t v; std::memcpy(&v, &r.v[lane], sizeof(v));
        const uint32_t exponent = (v >> 23) & 0xffu;
        if (exponent == 0) mac |= 1u << bit;
        else if (exponent == 0xff || (v & 0x7fffffffu) == 0x7f7fffffu) mac |= 1u << (bit + 12u);
        if (v & 0x80000000u) mac |= 1u << (bit + 4u);
    }
    uint32_t status = S.status & 0xff0u;
    if (mac & 0x000fu) status |= 0x001u | 0x040u;
    if (mac & 0x00f0u) status |= 0x002u | 0x080u;
    if (mac & 0xf000u) status |= 0x008u | 0x200u;
    S.mac = mac; S.status = status;
}
// broadcast lane: bc 0=x 1=y 2=z 3=w
inline Vu4 bcast(Vu4 s, uint32_t bc) {
    return {{s.v[bc & 3], s.v[bc & 3], s.v[bc & 3], s.v[bc & 3]}};
}
inline uint32_t laneOf(uint32_t f) { return f & 3; }  // fsf/ftf fields

// ---- SSE min/max mirrors: NaN picks the second operand ---------------------
inline float fmaxSSE(float a, float b) {
    if (std::isnan(a) || std::isnan(b)) return b;
    return a > b ? a : b;
}
inline float fminSSE(float a, float b) {
    if (std::isnan(a) || std::isnan(b)) return b;
    return a < b ? a : b;
}

// ---- oracle ps2_runtime_macros.h mirrors -----------------------------------
inline float vuDiv(float fs, float ft) {
    uint32_t bs, bt;
    std::memcpy(&bs, &fs, 4);
    std::memcpy(&bt, &ft, 4);
    if ((bt & 0x7FFFFFFFu) == 0) {
        float r;
        uint32_t bits = ((bs ^ bt) & 0x80000000u) | 0x7F7FFFFFu;
        std::memcpy(&r, &bits, 4);
        return r;
    }
    return fs / ft;
}
inline float vuSqrt(float ft) { return static_cast<float>(std::sqrt(std::fabs(static_cast<double>(ft)))); }
inline float vuRsqrt(float fs, float ft) {
    uint32_t bs, bt;
    std::memcpy(&bs, &fs, 4);
    std::memcpy(&bt, &ft, 4);
    if ((bt & 0x7FFFFFFFu) == 0) {
        uint32_t sign = (bs ^ bt) & 0x80000000u;
        float r;
        uint32_t bits = (bs & 0x7FFFFFFFu) == 0 ? sign : (sign | 0x7F7FFFFFu);
        std::memcpy(&r, &bits, 4);
        return r;
    }
    return fs / vuSqrt(ft);
}
// ps2_vu_ftoi: scale then trunc; tooBig (>= 2^31) pins 0x7FFFFFFF; SSE cvtt
// yields INT_MIN for NaN/negative overflow (shortcut: mirrors oracle guard,
// upgrade only if a golden ever exercises the NaN path).
inline float vuFtoiLanes(float s, float scale) {
    float x = s * scale;
    int32_t r;
    if (!(x < 2147483648.0f))
        r = 0x7FFFFFFF;  // tooBig (and +inf); NaN falls to SSE INT_MIN below
    else if (x < -2147483648.0f || std::isnan(x))
        r = static_cast<int32_t>(0x80000000u);
    else
        r = static_cast<int32_t>(static_cast<int64_t>(x));  // trunc toward 0
    float out;
    std::memcpy(&out, &r, 4);
    return out;
}

// ---- quad load/store (lqc2/sqc2 use MIPS base+ofs addresses) --------------
inline Vu4 lqc2(uint32_t addr) {
    Vu4 v;
    std::memcpy(&v, rtguest::G(addr), 16);
    return v;
}
inline void sqc2(uint32_t addr, Vu4 v) { std::memcpy(rtguest::G(addr), &v, 16); }

// ---- qmfc2 / qmtc2 / cfc2 / ctc2 ------------------------------------------
inline Vu4 qmfc2(uint32_t rd) { return S.vf[rd & 31]; }
inline void qmtc2(uint32_t rd, Vu4 v) { S.vf[rd & 31] = v; }

inline uint32_t cfc2(uint32_t cr) {
    if (cr < 16) return S.vi[cr];
    switch (cr) {
        case 16: return S.status;
        case 17: return S.mac;
        case 18: return S.clip & 0x00FFFFFFu;
        case 20: return S.r;
        case 21: { uint32_t b; std::memcpy(&b, &S.i_, 4); return b; }
        case 22: { uint32_t b; std::memcpy(&b, &S.q, 4); return b; }
        case 26: return S.tpc;
        case 27: return S.cmsar0;
        case 28: return S.fbrst;
        case 29: return S.vpu_stat;
        case 31: return S.cmsar1;
        default: return 0;
    }
}
inline void ctc2(uint32_t cr, uint32_t v) {
    if (cr < 16) { if (cr != 0) S.vi[cr] = static_cast<uint16_t>(v); return; }
    switch (cr) {
        case 16: S.status = v & 0xFFFFu; break;
        case 18: S.clip = v & 0x00FFFFFFu; break;
        case 20: S.r = v; break;  // oracle: set1_epi32; we keep the scalar
        case 21: { float f; std::memcpy(&f, &v, 4); S.i_ = f; break; }
        case 22: { float f; std::memcpy(&f, &v, 4); S.q = f; break; }
        default: break;  // read-only control regs ignored
    }
}

// ---- plain register ops (dest-masked) --------------------------------------
inline void vadd(uint32_t d, uint32_t fd, uint32_t fs, uint32_t ft) {
    Vu4 r{{S.vf[fs].v[0] + S.vf[ft].v[0], S.vf[fs].v[1] + S.vf[ft].v[1],
           S.vf[fs].v[2] + S.vf[ft].v[2], S.vf[fs].v[3] + S.vf[ft].v[3]}};
    S.vf[fd] = blend(S.vf[fd], r, d);
}
inline void vsub(uint32_t d, uint32_t fd, uint32_t fs, uint32_t ft) {
    Vu4 r{{S.vf[fs].v[0] - S.vf[ft].v[0], S.vf[fs].v[1] - S.vf[ft].v[1],
           S.vf[fs].v[2] - S.vf[ft].v[2], S.vf[fs].v[3] - S.vf[ft].v[3]}};
    S.vf[fd] = blend(S.vf[fd], r, d); fmacFlags(r, d);
}
inline void vmul(uint32_t d, uint32_t fd, uint32_t fs, uint32_t ft) {
    Vu4 r{{S.vf[fs].v[0] * S.vf[ft].v[0], S.vf[fs].v[1] * S.vf[ft].v[1],
           S.vf[fs].v[2] * S.vf[ft].v[2], S.vf[fs].v[3] * S.vf[ft].v[3]}};
    S.vf[fd] = blend(S.vf[fd], r, d);
}

// ---- bc (broadcast-second-source) forms ------------------------------------
inline void vaddbc(uint32_t d, uint32_t bc, uint32_t fd, uint32_t fs, uint32_t ft) {
    Vu4 b = bcast(S.vf[ft], bc), &s = S.vf[fs];
    Vu4 r{{s.v[0] + b.v[0], s.v[1] + b.v[1], s.v[2] + b.v[2], s.v[3] + b.v[3]}};
    S.vf[fd] = blend(S.vf[fd], r, d);
}
inline void vsubbc(uint32_t d, uint32_t bc, uint32_t fd, uint32_t fs, uint32_t ft) {
    Vu4 b = bcast(S.vf[ft], bc), &s = S.vf[fs];
    Vu4 r{{s.v[0] - b.v[0], s.v[1] - b.v[1], s.v[2] - b.v[2], s.v[3] - b.v[3]}};
    S.vf[fd] = blend(S.vf[fd], r, d); fmacFlags(r, d);
}
inline void vmulbc(uint32_t d, uint32_t bc, uint32_t fd, uint32_t fs, uint32_t ft) {
    Vu4 b = bcast(S.vf[ft], bc), &s = S.vf[fs];
    Vu4 r{{s.v[0] * b.v[0], s.v[1] * b.v[1], s.v[2] * b.v[2], s.v[3] * b.v[3]}};
    S.vf[fd] = blend(S.vf[fd], r, d);
}
inline void vmaddbc(uint32_t d, uint32_t bc, uint32_t fd, uint32_t fs, uint32_t ft) {
    Vu4 b = bcast(S.vf[ft], bc), &s = S.vf[fs], &a = S.acc;
    Vu4 r{{a.v[0] + s.v[0] * b.v[0], a.v[1] + s.v[1] * b.v[1],
           a.v[2] + s.v[2] * b.v[2], a.v[3] + s.v[3] * b.v[3]}};
    S.vf[fd] = blend(S.vf[fd], r, d);
}
inline void vmaxbc(uint32_t d, uint32_t bc, uint32_t fd, uint32_t fs, uint32_t ft) {
    Vu4 b = bcast(S.vf[ft], bc), &s = S.vf[fs];
    Vu4 r{{fmaxSSE(s.v[0], b.v[0]), fmaxSSE(s.v[1], b.v[1]),
           fmaxSSE(s.v[2], b.v[2]), fmaxSSE(s.v[3], b.v[3])}};
    S.vf[fd] = blend(S.vf[fd], r, d);
}
inline void vminibc(uint32_t d, uint32_t bc, uint32_t fd, uint32_t fs, uint32_t ft) {
    Vu4 b = bcast(S.vf[ft], bc), &s = S.vf[fs];
    Vu4 r{{fminSSE(s.v[0], b.v[0]), fminSSE(s.v[1], b.v[1]),
           fminSSE(s.v[2], b.v[2]), fminSSE(s.v[3], b.v[3])}};
    S.vf[fd] = blend(S.vf[fd], r, d);
}
// vmulabc/vmaddabc write ACC (fd field absent in the encoding):
inline void vmulabc(uint32_t d, uint32_t bc, uint32_t fs, uint32_t ft) {
    Vu4 b = bcast(S.vf[ft], bc), &s = S.vf[fs];
    Vu4 r{{s.v[0] * b.v[0], s.v[1] * b.v[1], s.v[2] * b.v[2], s.v[3] * b.v[3]}};
    S.acc = blend(S.acc, r, d);
}
inline void vmaddabc(uint32_t d, uint32_t bc, uint32_t fs, uint32_t ft) {
    Vu4 b = bcast(S.vf[ft], bc), &s = S.vf[fs], &a = S.acc;
    Vu4 r{{a.v[0] + s.v[0] * b.v[0], a.v[1] + s.v[1] * b.v[1],
           a.v[2] + s.v[2] * b.v[2], a.v[3] + s.v[3] * b.v[3]}};
    S.acc = blend(S.acc, r, d);
}

// ---- Q/I scalar-source ops --------------------------------------------------
inline void vaddq(uint32_t d, uint32_t fd, uint32_t fs) {
    Vu4 r{{S.vf[fs].v[0] + S.q, S.vf[fs].v[1] + S.q, S.vf[fs].v[2] + S.q, S.vf[fs].v[3] + S.q}};
    S.vf[fd] = blend(S.vf[fd], r, d);
}
inline void vmulq(uint32_t d, uint32_t fd, uint32_t fs) {
    Vu4 r{{S.vf[fs].v[0] * S.q, S.vf[fs].v[1] * S.q, S.vf[fs].v[2] * S.q, S.vf[fs].v[3] * S.q}};
    S.vf[fd] = blend(S.vf[fd], r, d);
}

// ---- Q pipeline (vwaitq is a no-op: our Q resolves immediately) -------------
inline void vdiv(uint32_t fsf, uint32_t fs, uint32_t ftf, uint32_t ft) {
    S.q = vuDiv(S.vf[fs].v[laneOf(fsf)], S.vf[ft].v[laneOf(ftf)]);
}
inline void vsqrt(uint32_t ftf, uint32_t ft) { S.q = vuSqrt(S.vf[ft].v[laneOf(ftf)]); }
inline void vwaitq() {}

// ---- ACC cross products (VU0 macro OPMLA/OPMS, dest masked) -----------------
inline void vopmula(uint32_t d, uint32_t fs, uint32_t ft) {
    // oracle shuffles: fs.yzxw * ft.zxyw lane-wise (no ACC add), dest-masked
    const float *a = S.vf[fs].v, *b = S.vf[ft].v;
    Vu4 r{{a[1] * b[2], a[2] * b[0], a[0] * b[1], a[3] * b[3]}};
    S.acc = blend(S.acc, r, d);
}
inline void vopmsub(uint32_t d, uint32_t fd, uint32_t fs, uint32_t ft) {
    // oracle: fd = ACC - (fs yzx * ft zxy) lane-wise; lane3 = acc.w - a.w*b.w
    const float *a = S.vf[fs].v, *b = S.vf[ft].v, *ac = S.acc.v;
    Vu4 r{{ac[0] - a[1] * b[2], ac[1] - a[2] * b[0], ac[2] - a[0] * b[1],
           ac[3] - a[3] * b[3]}};
    S.vf[fd] = blend(S.vf[fd], r, d);
}

// ---- conversions -------------------------------------------------------------
inline void vftoi(uint32_t d, uint32_t ft, uint32_t fs, float scale) {
    Vu4 r{{vuFtoiLanes(S.vf[fs].v[0], scale), vuFtoiLanes(S.vf[fs].v[1], scale),
           vuFtoiLanes(S.vf[fs].v[2], scale), vuFtoiLanes(S.vf[fs].v[3], scale)}};
    S.vf[ft] = blend(S.vf[ft], r, d);
}
inline void vftoi0(uint32_t d, uint32_t ft, uint32_t fs) { vftoi(d, ft, fs, 1.f); }
inline void vftoi4(uint32_t d, uint32_t ft, uint32_t fs) { vftoi(d, ft, fs, 16.f); }
inline void vftoi15(uint32_t d, uint32_t ft, uint32_t fs) { vftoi(d, ft, fs, 32768.f); }
inline void vitof(uint32_t d, uint32_t ft, uint32_t fs, float scale) {
    Vu4 r;
    for (int l = 0; l < 4; ++l) {
        int32_t bits;
        std::memcpy(&bits, &S.vf[fs].v[l], 4);
        r.v[l] = static_cast<float>(bits) * scale;
    }
    S.vf[ft] = blend(S.vf[ft], r, d);
}
inline void vitof0(uint32_t d, uint32_t ft, uint32_t fs) { vitof(d, ft, fs, 1.f); }
inline void vitof4(uint32_t d, uint32_t ft, uint32_t fs) { vitof(d, ft, fs, 1.f / 16.f); }
inline void vitof15(uint32_t d, uint32_t ft, uint32_t fs) { vitof(d, ft, fs, 1.f / 32768.f); }

// ---- vi transfer / integer ops ----------------------------------------------
inline void vmtir(uint32_t ft, uint32_t fsf, uint32_t fs) {
    float src = S.vf[fs].v[laneOf(fsf)];
    uint32_t bits;
    std::memcpy(&bits, &src, 4);
    S.vi[ft] = static_cast<uint16_t>(bits & 0xFFFFu);
}
inline void vmfir(uint32_t d, uint32_t ft, uint32_t fs) {
    uint32_t tmp = static_cast<uint32_t>(static_cast<int32_t>(static_cast<int16_t>(S.vi[fs])));
    float val;
    std::memcpy(&val, &tmp, 4);
    S.vf[ft] = blend(S.vf[ft], Vu4{{val, val, val, val}}, d);
}
inline void viadd(uint32_t fd, uint32_t fs, uint32_t ft) { S.vi[fd] = static_cast<uint16_t>(S.vi[fs] + S.vi[ft]); }
inline void vior(uint32_t fd, uint32_t fs, uint32_t ft) { S.vi[fd] = static_cast<uint16_t>(S.vi[fs] | S.vi[ft]); }

// ---- indexed quads (vi-addressed; oracle: (vi & 0x3FF) << 4) -----------------
inline Vu4 vlqi(uint32_t ft, uint32_t fs) {  // post-increment
    uint32_t addr = (static_cast<uint32_t>(S.vi[fs] & 0x3FFu)) << 4;
    S.vi[fs] = static_cast<uint16_t>((S.vi[fs] + 1) & 0x3FFu);
    return lqc2(addr);
}
inline void vsqi(uint32_t d, uint32_t fs, uint32_t ft) {  // store vf[fs], index vi[ft]
    uint32_t addr = (static_cast<uint32_t>(S.vi[ft] & 0x3FFu)) << 4;
    Vu4 old = lqc2(addr);
    sqc2(addr, blend(old, S.vf[fs], d));
    S.vi[ft] = static_cast<uint16_t>((S.vi[ft] + 1) & 0x3FFu);
}

// ---- misc ---------------------------------------------------------------------
inline void vmove(uint32_t d, uint32_t ft, uint32_t fs) { S.vf[ft] = blend(S.vf[ft], S.vf[fs], d); }
inline void vmr32(uint32_t d, uint32_t ft, uint32_t fs) {  // SSE shuffle (0,3,2,1): y,z,w,x
    const float *s = S.vf[fs].v;
    S.vf[ft] = blend(S.vf[ft], Vu4{{s[1], s[2], s[3], s[0]}}, d);
}
inline void vnop() {}

}  // namespace rtguest::vu0
