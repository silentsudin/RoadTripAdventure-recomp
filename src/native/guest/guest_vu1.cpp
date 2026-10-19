// W3 gs-sink step 2: transliteration of Road Trip's slot VU1 programs (see header).
// Blocks cite micro-op ranges of build/scratch/w3-step2/entry{1,2,5}_*.dis
// (image hash-verified FNV-1a 0xe2868a5859386847 against the oracle's VU1 image).
// Verified per run against build/vu1hle-caps captures (vu1_strips_test).

#include "guest_vu1.h"
#include "guest_vif1.h"

#include <cfenv>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <limits>
#include <vector>

namespace rtguest {
namespace {

constexpr std::uint32_t kQwordMask = 0x3FFu;  // VIF1-addressable VU1 data: 1024 quads

// VU1 data-memory layout the programs share (qword addresses; dis ranges cited
// where the programs touch them).
namespace mem {
constexpr std::uint32_t kWorld = 0;          // object matrix columns; SetupLit overwrites with world->screen (dis1 1590-15b0)
constexpr std::uint32_t kCamera = 4;         // world->screen (camera x viewport)
constexpr std::uint32_t kClip = 8;           // world->clip (guard-band space)
constexpr std::uint32_t kClipToScreen = 12;  // clip->screen
constexpr std::uint32_t kLightDirs = 16;     // light 3x3 rows
constexpr std::uint32_t kFogLit = 19;        // fog/clamp params (dis2 1720/1748/1770/17c0: far/scale/max)
constexpr std::uint32_t kLightColours = 20;  // light0, light1, specular, ambient
constexpr std::uint32_t kTint = 24;          // tint (LitStrip) / colour scale (OffsetStrip)
constexpr std::uint32_t kWeights = 25;       // four cycling offset weights (OffsetStrip dis5 loop)
constexpr std::uint32_t kHeaderOffset = 402; // TOP+402: batch state packet header (dis2 1628)
}  // namespace mem

// ---- VU1 lane arithmetic -------------------------------------------------------------
// normalizeOperand (.inl:194): denormals -> signed zero, Inf/NaN -> signed max.
inline float normIn(float v) {
    std::uint32_t b;
    std::memcpy(&b, &v, 4);
    const std::uint32_t exp = b & 0x7F800000u;
    const std::uint32_t sign = b & 0x80000000u;
    if (exp == 0u) b = sign;
    else if (exp == 0x7F800000u) b = sign | 0x7F7FFFFFu;
    std::memcpy(&v, &b, 4);
    return v;
}

// Single round toward zero of the exact double value (VU1 FMAC/DIV: ps2_vu1_exec.inl
// header: "the host result is the exact value rounded once toward zero"). Compiler-
// independent: no reliance on host FE_TOWARDZERO or FMA contraction.
inline float rz(double x) {
    if (!(x == x)) return x - x;  // NaN
    if (x >= 0x1.0p+128) return HUGE_VALF;
    if (x <= -0x1.0p+128) return -HUGE_VALF;
    const float rne = static_cast<float>(x);  // host RNE
    if (rne == x) return rne;
    if (std::fabs(static_cast<double>(rne)) > std::fabs(x))
        return std::nextafterf(rne, 0.0f);  // RNE rounded away from zero: step back
    return rne;
}

// normalizeResult value part (.inl:575 via normalizeFmacExactResult): underflow ->
// signed zero (exact sign), overflow -> signed max.
inline float normOut(float value, double exact) {
    const std::uint32_t sign = std::signbit(exact) ? 0x80000000u : 0u;
    const double mag = std::fabs(exact);
    std::uint32_t b;
    if (mag == 0.0 || mag < static_cast<double>(std::numeric_limits<float>::min())) {
        std::memcpy(&value, &sign, 4);
    } else if (mag > static_cast<double>(std::numeric_limits<float>::max())) {
        b = sign | 0x7F7FFFFFu;
        std::memcpy(&value, &b, 4);
    }
    return value;
}

inline float fadd(float a, float b) {
    a = normIn(a); b = normIn(b);
    const double e = static_cast<double>(a) + static_cast<double>(b);
    return normOut(rz(e), e);
}
inline float fsub(float a, float b) {
    a = normIn(a); b = normIn(b);
    const double e = static_cast<double>(a) - static_cast<double>(b);
    return normOut(rz(e), e);
}
inline float fmul(float a, float b) {
    a = normIn(a); b = normIn(b);
    const double e = static_cast<double>(a) * static_cast<double>(b);
    return normOut(rz(e), e);
}
inline float fmadd(float acc, float a, float b) {
    acc = normIn(acc); a = normIn(a); b = normIn(b);
    const double e = static_cast<double>(acc) + static_cast<double>(a) * static_cast<double>(b);
    return normOut(rz(e), e);
}
// MAX/MINI compare the normalised operands, result not renormalised (.inl max/min).
inline float fmax(float a, float b) {
    a = normIn(a); b = normIn(b);
    return a > b ? a : b;
}
inline float fmin(float a, float b) {
    a = normIn(a); b = normIn(b);
    return a < b ? a : b;
}
// DIV -> Q (.inl:1149): zero denominator gives +-max.
inline float fdiv(float num, float den) {
    num = normIn(num); den = normIn(den);
    if (den == 0.0f)
        return std::signbit(num) != std::signbit(den) ? -std::numeric_limits<float>::max()
                                                      : std::numeric_limits<float>::max();
    const double e = static_cast<double>(num) / static_cast<double>(den);
    return normOut(rz(e), e);
}
// FTOIn (.inl:1922): scale in double, truncate, saturate; stored as int bits.
inline std::uint32_t ftoi(float v, double scale) {
    const double s = static_cast<double>(normIn(v)) * scale;
    std::int32_t i;
    if (s >= static_cast<double>(std::numeric_limits<std::int32_t>::max())) i = std::numeric_limits<std::int32_t>::max();
    else if (s <= static_cast<double>(std::numeric_limits<std::int32_t>::min())) i = std::numeric_limits<std::int32_t>::min();
    else i = static_cast<std::int32_t>(s);
    return static_cast<std::uint32_t>(i);
}
// CLIP (.inl:1106): six flags v.xyz vs |vt.w|'s exponent field, on raw bits
// (bit 0 +x, 1 -x, 2 +y, 3 -y, 4 +z, 5 -z). LitStrip dis2 17a8: CLIP f24, f24.
inline std::uint32_t clipFlags(const float* v, const float* wReg) {
    std::uint32_t wBits;
    std::memcpy(&wBits, &wReg[3], 4);
    const std::int32_t limit = (wBits & 0x7F800000u) != 0u
                                   ? static_cast<std::int32_t>(wBits & 0x7FFFFFFFu)
                                   : 0x007FFFFF;
    std::uint32_t flags = 0u;
    for (int lane = 0; lane < 3; ++lane) {
        std::uint32_t b;
        std::memcpy(&b, &v[lane], 4);
        if (static_cast<std::int32_t>(b) > limit) flags |= 1u << (2 * lane);
        if (static_cast<std::int32_t>(b ^ 0x80000000u) > limit) flags |= 2u << (2 * lane);
    }
    return flags;
}

// ---- program scratch state -------------------------------------------------------------
Vu1Regs g_vu;
Vu1KickFn g_kick = nullptr;
std::uint64_t g_runs = 0, g_unported = 0;
std::uint32_t g_top = 0;

std::uint8_t* qword(std::uint32_t addr) { return vu1Data() + (addr & kQwordMask) * 16u; }
inline void loadQ(std::uint32_t addr, float* v) { std::memcpy(v, qword(addr), 16); }
inline void storeQ(std::uint32_t addr, const void* v) { std::memcpy(qword(addr), v, 16); }
inline std::int16_t half(std::uint32_t addr, int lane) {  // ILW: low 16 bits sign-extended (VI is 16-bit)
    std::uint32_t w;
    std::memcpy(&w, qword(addr) + lane * 4, 4);
    return static_cast<std::int16_t>(w & 0xFFFFu);
}
inline void setHalf(std::uint32_t addr, int lane, std::uint16_t v) {  // ISW: zero-extended into the word
    const std::uint32_t w = v;
    std::memcpy(qword(addr) + lane * 4, &w, 4);
}

// XGKICK GIF-tag layout as the interpreter's transfer reads it (.inl:807
// progressXgkick): NLOOP = tag bits 14:0, EOP bit 15, format bits 59:58, NREG bits
// 63:60 (0 = 16). Copies the packet at addr through the first EOP tag.
void kickPacket(std::uint32_t addr) {
    if (!g_kick) return;
    std::vector<std::uint8_t> packet;
    std::uint32_t q = addr;
    for (;;) {
        packet.insert(packet.end(), qword(q), qword(q) + 16);
        std::uint64_t tag;
        std::memcpy(&tag, qword(q), 8);
        ++q;
        const std::uint64_t nloop = tag & 0x7FFFu;
        const std::uint32_t format = static_cast<std::uint32_t>((tag >> 58) & 3u);
        std::uint64_t nreg = (tag >> 60) & 0xFu;
        if (nreg == 0u) nreg = 16u;
        std::uint64_t qwords = 0;
        if (format == 0u) qwords = nloop * nreg;
        else if (format == 1u) qwords = (nloop * nreg + 1u) / 2u;
        else if (format == 2u) qwords = nloop;
        else return;  // the interpreter drops such a transfer (never seen)
        for (std::uint64_t i = 0; i < qwords; ++i, ++q)
            packet.insert(packet.end(), qword(q), qword(q) + 16);
        if ((tag >> 15) & 1u) break;
        if (packet.size() >= 128u * 1024u) return;  // shortcut: runaway tag guard, matches interpreter buffer cap intent
    }
    g_kick(packet.data(), static_cast<std::uint32_t>(packet.size()));
}

// Matrix as the microcode keeps it: four column quads. The vertex loops chain
// MULAx, MADDAy, MADDAz, MADDw (dis2 1670-1688 etc.), so the transforms do too.
struct Mat { float col[4][4]; };
inline void transformColAcc(float acc[4], const float col[4], float s) {
    for (int l = 0; l < 4; ++l) acc[l] = fmadd(acc[l], col[l], s);
}
inline void transform(float outv[4], const Mat& m, const float v[4]) {
    for (int l = 0; l < 4; ++l) outv[l] = fmul(m.col[0][l], v[0]);
    transformColAcc(outv, m.col[1], v[1]);
    transformColAcc(outv, m.col[2], v[2]);
    transformColAcc(outv, m.col[3], v[3]);
}
inline void transformPoint(float outv[4], const Mat& m, const float v[4]) {  // v.w taken as 1 (dis2 1688 MADD f25, f4, wf0)
    for (int l = 0; l < 4; ++l) outv[l] = fmul(m.col[0][l], v[0]);
    transformColAcc(outv, m.col[1], v[1]);
    transformColAcc(outv, m.col[2], v[2]);
    for (int l = 0; l < 4; ++l) outv[l] = fmadd(outv[l], m.col[3][l], 1.0f);
}

// ---- Program 0: Init (slot pc 0x00) ------------------------------------------------------
// Points both double-buffer halves' batch headers as sent, loads matrices and
// lights straight from data memory. (dis 0x0000-0x0010 table slot; the setup
// stores' source words are the UNPACKed constants at mem:: addresses.)
void runInit() {
    setHalf(141 + mem::kHeaderOffset, 0, 0);  // TOP 141 half: NLOOP cleared
    setHalf(571 + mem::kHeaderOffset, 0, 0);  // TOP 571 half
    for (int i = 0; i < 4; ++i) {
        loadQ(mem::kClipToScreen + i, g_vu.vf[1 + i]);
        loadQ(mem::kClip + i, g_vu.vf[13 + i]);
    }
    for (int i = 0; i < 3; ++i) {
        loadQ(mem::kLightDirs + i, g_vu.vf[5 + i]);
        loadQ(mem::kLightColours + i, g_vu.vf[9 + i]);  // VF9..11 = VU19..21
    }
    g_vu.vi[14] = 1024;  // register-save stack pointer (wrap base)
}

// ---- Program 1: SetupLit (slot pc 0x10; program 0x1420-0x1618) ----------------------------
// world->screen = camera x world (written back over the world matrix), world->clip
// = clip x world, light directions into object space (light x world's rotation).
void runSetupLit() {
    Mat world, camera, clip, light;
    for (int i = 0; i < 4; ++i) {
        loadQ(mem::kWorld + i, world.col[i]);
        loadQ(mem::kCamera + i, camera.col[i]);
        loadQ(mem::kClip + i, clip.col[i]);
    }
    for (int i = 0; i < 3; ++i) loadQ(mem::kLightDirs + i, light.col[i]);
    for (int l = 0; l < 4; ++l) light.col[3][l] = l == 3 ? 1.0f : 0.0f;  // VF0.w column (dis1 col3 chain)
    Mat screen, clipW;
    for (int i = 0; i < 4; ++i) {
        transform(screen.col[i], camera, world.col[i]);
        transform(clipW.col[i], clip, world.col[i]);
        std::memcpy(g_vu.vf[1 + i], screen.col[i], 16);
        std::memcpy(g_vu.vf[13 + i], clipW.col[i], 16);
    }
    for (int i = 0; i < 3; ++i) {
        float v[4];
        transform(v, light, world.col[i]);
        std::memcpy(g_vu.vf[5 + i], v, 16);
    }
    for (int i = 0; i < 4; ++i) storeQ(mem::kWorld + i, g_vu.vf[1 + i]);  // world := world->screen
    loadQ(mem::kFogLit, g_vu.vf[8]);
    for (int i = 0; i < 4; ++i) loadQ(mem::kLightColours + i, g_vu.vf[9 + i]);
    loadQ(mem::kTint, g_vu.vf[17]);
    loadQ(mem::kWeights, g_vu.vf[18]);
    float tmp[4];
    loadQ(mem::kClip + 2, tmp);
    g_vu.vf[27][3] = tmp[3];  // temporaries keeping clip columns
    loadQ(mem::kClip + 3, tmp);
    g_vu.vf[28][3] = tmp[3];
    g_vu.vi[14] = 1024;
}

// Strip bookkeeping both strips share (dis2 17d0-1810 FCAND/IADD window + IBLEZ
// loop-back; dis2 1820-1830 ISW ADC patch). A primitive closes at each vertex from
// the third on and is marked ADC when any of its three vertices is outside the
// clip volume; the first two vertices start at -2 so they never count.
struct OutsideWindow {
    std::int16_t older = -2, old = -2;
    std::int16_t push(std::int16_t outside) {
        const std::int16_t sum = static_cast<std::int16_t>(older + old + outside);
        older = old;
        old = outside;
        return sum;
    }
};

constexpr std::uint32_t kInStride = 4;
constexpr std::uint16_t kAdcNoFog = 0x8FF0;  // XYZF2.w: ADC set, F = 0xFF (dis2 1820-1830: -1, -15 -> value 0x8FF0 at the qword three vertices back)

inline void setBits(float& dst, std::uint32_t b) { std::memcpy(&dst, &b, 4); }  // FTOI/STQ carriers hold integer bit patterns

std::uint32_t headerAddr() { return (g_top + mem::kHeaderOffset) & 0xFFFFu; }

void sendBatch(std::uint32_t header, std::uint32_t packet) {
    // Header first; its NLOOP clears after the kick snapshot took it (dis2 1858/
    // 1860/1868: XGKICK vi13, XGKICK vi2, ISWR vi13).
    if (header != 0u) {
        kickPacket(header);
        setHalf(header, 0, 0);
    }
    kickPacket(packet);
}

// ---- Program 2: LitStrip (slot pc 0x20; program 0x15F8-0x1890) --------------------------
// Vertex record: position, normal, colour, (s, t, spec weight). Registers from
// SetupLit: VF1..4 world->screen, VF13..16 world->clip, VF5..7 light dirs (object
// space, as columns), VF9/10 light colours, VF11 specular, VF12 ambient (w alpha
// cap), VF8 fog (w far, z scale, y colour cap), VF17/18 tints picked by tag w.
void runLitStrip() {
    const std::uint32_t header = half(headerAddr(), 0) != 0 ? headerAddr() : 0u;
    const std::uint32_t count = static_cast<std::uint32_t>(half(g_top, 0)) & 0x7FFFu;  // dis2 1658 ILWR vi10
    if (count == 0u) return;  // the microcode would loop 65536 times
    const std::int16_t tintSelect = half(g_top, 3);  // dis2 1668 ILWR vi8: tag quad's W word
    Mat screen, clipM, lights;
    for (int i = 0; i < 4; ++i) {
        std::memcpy(screen.col[i], g_vu.vf[1 + i], 16);
        std::memcpy(clipM.col[i], g_vu.vf[13 + i], 16);
    }
    for (int i = 0; i < 4; ++i)
        for (int l = 0; l < 4; ++l) lights.col[i][l] = i < 3 ? g_vu.vf[5 + i][l] : 0.0f;
    const float* fog = g_vu.vf[8];
    const float *light0 = g_vu.vf[9], *light1 = g_vu.vf[10], *specular = g_vu.vf[11], *ambient = g_vu.vf[12];

    // Tint: none (0+VF0.w=1) unless the tag's W word says VF18 (even) / VF17 (odd).
    // dis2 1690 ADD.xyz f22, f0, wf0 / 16d8 +17 / 16f8 +18.
    float tint[4];
    for (int l = 0; l < 3; ++l) tint[l] = fadd(0.0f, 1.0f);
    if (tintSelect != 0) {
        const float* pick = (tintSelect & 1) ? g_vu.vf[17] : g_vu.vf[18];
        for (int l = 0; l < 3; ++l) tint[l] = fadd(0.0f, pick[l]);
    }

    const std::uint32_t in = g_top + 1;
    const std::uint32_t out = in + count * kInStride;
    storeQ(out, qword(in - 1));  // dis2 1670 LQ f30, 0(vi1); SQ f30, 0(vi2): the GIF tag quad
    std::uint32_t stqW;
    std::memcpy(&stqW, &g_vu.vf[28][3], 4);  // STQ.w carries the previous vertex's fog word (dis2 FTOI4 wf28 chain)
    std::uint32_t lastXyzf[4] = {};
    OutsideWindow window;
    float pos[4], normal[4], colour[4], st[4], hpos[4], cpos[4], d[4], lit[4], rgba[4], o[3][4];
    for (std::uint32_t k = 0; k < count; ++k) {
        const std::uint32_t rec = in + k * kInStride;
        loadQ(rec, pos); loadQ(rec + 1, normal); loadQ(rec + 2, colour); loadQ(rec + 3, st);

        transformPoint(hpos, screen, pos);                        // dis2 1670-1688
        g_vu.q = fdiv(1.0f, hpos[3]);                             // dis2 1700 DIV -> Q; 1780 MUL q
        const float q = g_vu.q;
        float f = fsub(fog[3], hpos[3]);                          // fog: 1700 SUB.w, 1720 MUL.z zf8,
        f = fmul(f, fog[2]);
        f = fmax(f, 0.0f);                                        // 1748 MAX.w yf0
        f = fmin(f, fog[1]);                                      // 1770 MINI.w yf8
        transformPoint(cpos, clipM, pos);                         // dis2 1708-1730
        const std::uint32_t outside = clipFlags(cpos, cpos) != 0u ? 1u : 0u;  // dis2 17a8 CLIP f24,f24

        // Diffuse max(L.n, 0) for two lights + ambient: MULAA/MADDAA chains
        // 1698-1708 (n.x/n.y) and 1740-1758 (n.z via f29), clamps 1718.
        for (int l = 0; l < 4; ++l) d[l] = fmul(lights.col[0][l], normal[0]);
        transformColAcc(d, lights.col[1], normal[1]);
        transformColAcc(d, lights.col[2], normal[2]);
        for (int l = 0; l < 3; ++l) d[l] = fmax(d[l], 0.0f);
        float spec = d[2];                                        // specular weight: ^8 (MUL.z x3, 1738/1758/1788)
        spec = fmul(spec, spec);
        spec = fmul(spec, spec);
        spec = fmul(spec, spec);
        for (int l = 0; l < 3; ++l)                               // light0*d0 + light1*d1 + ambient (dis2 1760 f26 chain, MADD wf0)
            lit[l] = fmadd(fmadd(fmul(light0[l], d[0]), light1[l], d[1]), ambient[l], 1.0f);

        for (int l = 0; l < 3; ++l) {                             // colour*tint*lit + spec*st.z*highlight, capped fog.y (1778/17b0/17c0)
            const float highlight = fmul(fmul(specular[l], st[2]), spec);
            float c = fmul(colour[l], tint[l]);
            c = fmul(c, lit[l]);
            c = fadd(c, highlight);
            rgba[l] = fmin(c, fog[1]);
        }
        rgba[3] = fmin(f, ambient[3]);                            // 17c8 MINI.w f25, f12

        for (int l = 0; l < 3; ++l) o[0][l] = fmul(l == 2 ? 1.0f : st[l], q);  // STQ (dis2 1790 MUL f28 w/q, 17f0..; st.z*1: 1780)
        setBits(o[0][3], stqW);
        for (int l = 0; l < 4; ++l) setBits(o[1][l], ftoi(rgba[l], 1.0));              // RGBAQ: FTOI0 (17e8)
        for (int l = 0; l < 3; ++l) hpos[l] = fmul(hpos[l], q);                        // XYZF2: perspective (1780)
        hpos[3] = f;
        for (int l = 0; l < 4; ++l) setBits(o[2][l], lastXyzf[l] = ftoi(hpos[l], 16.0));  // FTOI4 (17b8)
        stqW = lastXyzf[3];

        const std::uint32_t at = out + 1 + 3 * k;                 // SQ f28/-4..-2(f30 f19): dis2 17b0/17e8/1808
        storeQ(at, o[0]);
        storeQ(at + 1, o[1]);
        storeQ(at + 2, o[2]);
        if (window.push(static_cast<std::int16_t>(outside)) > 0)
            setHalf(at + 2, 3, kAdcNoFog);                        // dis2 1820-1830 ISW ADC patch
    }
    for (int l = 0; l < 4; ++l)
        std::memcpy(&g_vu.vf[28][l], &lastXyzf[l], 4);            // VF28 = last XYZF2 bits (carry to next batch)
    float last[4];
    loadQ(in + (count - 1) * kInStride + 3, last);
    g_vu.vf[27][3] = last[3];                                     // VF27.w = last record's W
    sendBatch(header, out);                                       // dis2 1858-1880 XGKICK vi13/vi2
}

// ---- Program 5: OffsetStrip (slot pc 0x50; program 0x1898-0x1930) -------------------------
// Vertex record: position, (unused), colour, (s, t, depth scale). Each vertex moves
// along the screen/clip matrices' Y column by one of four cycling weights (VF18);
// colour x tint (VF24 word), alpha = tint.w x fog clamped to VF8.x; screen Z
// scaled per vertex. No lighting.
void runOffsetStrip() {
    const std::uint32_t header = half(headerAddr(), 0) != 0 ? headerAddr() : 0u;
    const std::uint32_t count = static_cast<std::uint32_t>(half(g_top, 0)) & 0x7FFFu;
    if (count == 0u) return;
    Mat screen, clipM;
    for (int i = 0; i < 4; ++i) {
        std::memcpy(screen.col[i], g_vu.vf[1 + i], 16);
        std::memcpy(clipM.col[i], g_vu.vf[13 + i], 16);
    }
    const float* fog = g_vu.vf[8];
    float tint[4], weights[4], screenAxis[4], clipAxis[4];
    loadQ(mem::kTint, tint);
    loadQ(mem::kWeights, weights);
    loadQ(mem::kCamera + 1, screenAxis);  // VU5: matrices' Y column (the offset axis)
    loadQ(mem::kClip + 1, clipAxis);      // VU9

    const std::uint32_t in = g_top + 1;
    const std::uint32_t out = in + count * kInStride;
    storeQ(out, qword(in - 1));
    std::uint32_t lastXyzf[4] = {};
    OutsideWindow window;
    float pos[4], colour[4], st[4], hpos[4], cpos[4], rgba[4], o[3][4];
    for (std::uint32_t k = 0; k < count; ++k) {
        const std::uint32_t rec = in + k * kInStride;
        loadQ(rec, pos); loadQ(rec + 2, colour); loadQ(rec + 3, st);
        const float offset = weights[0];

        // offsetTransform: acc = axis*k, then columns x pos (dis5 loop MADDA chain).
        for (int l = 0; l < 4; ++l) hpos[l] = fmul(screenAxis[l], offset);
        transformColAcc(hpos, screen.col[0], pos[0]);
        transformColAcc(hpos, screen.col[1], pos[1]);
        transformColAcc(hpos, screen.col[2], pos[2]);
        for (int l = 0; l < 4; ++l) hpos[l] = fmadd(hpos[l], screen.col[3][l], 1.0f);
        g_vu.q = fdiv(1.0f, hpos[3]);
        const float q = g_vu.q;
        for (int l = 0; l < 4; ++l) cpos[l] = fmul(clipAxis[l], offset);
        transformColAcc(cpos, clipM.col[0], pos[0]);
        transformColAcc(cpos, clipM.col[1], pos[1]);
        transformColAcc(cpos, clipM.col[2], pos[2]);
        for (int l = 0; l < 4; ++l) cpos[l] = fmadd(cpos[l], clipM.col[3][l], 1.0f);
        const std::uint32_t outside = clipFlags(cpos, cpos) != 0u ? 1u : 0u;

        float f = fsub(fog[3], hpos[3]);
        f = fmul(f, fog[2]);
        f = fmax(f, 0.0f);
        const float alphaFog = fmin(f, fog[0]);  // alpha clamped to VF8.x (distinct from the colour cap)
        f = fmin(f, fog[1]);

        for (int l = 0; l < 3; ++l) hpos[l] = fmul(hpos[l], q);
        hpos[2] = fmul(hpos[2], st[2]);          // screen Z scaled per vertex
        hpos[3] = f;

        for (int l = 0; l < 3; ++l) o[0][l] = fmul(l == 2 ? 1.0f : st[l], q);
        std::memcpy(&o[0][3], &st[3], 4);  // OffsetStrip passes the record's W lane through
        for (int l = 0; l < 3; ++l) rgba[l] = fmul(colour[l], tint[l]);
        rgba[3] = fmul(tint[3], alphaFog);
        for (int l = 0; l < 4; ++l) {
            setBits(o[1][l], ftoi(rgba[l], 1.0));
            setBits(o[2][l], lastXyzf[l] = ftoi(hpos[l], 16.0));
        }

        const std::uint32_t at = out + 1 + 3 * k;
        storeQ(at, o[0]);
        storeQ(at + 1, o[1]);
        storeQ(at + 2, o[2]);
        if (window.push(static_cast<std::int16_t>(outside)) > 0)
            setHalf(at + 2, 3, kAdcNoFog);

        const float w0 = weights[0];  // MR32 weights cycle (dis5 loop MR32: xyzw -> yzwx)
        weights[0] = weights[1];
        weights[1] = weights[2];
        weights[2] = weights[3];
        weights[3] = w0;
    }
    std::memcpy(g_vu.vf[10], screenAxis, 16);  // temporaries written back (dis5 tail LQ/MOV chains)
    std::memcpy(g_vu.vf[11], clipAxis, 16);
    std::memcpy(g_vu.vf[17], tint, 16);
    std::memcpy(g_vu.vf[18], weights, 16);
    for (int l = 0; l < 4; ++l)
        std::memcpy(&g_vu.vf[28][l], &lastXyzf[l], 4);
    float last[4];
    loadQ(in + (count - 1) * kInStride + 3, last);
    g_vu.vf[27][3] = last[3];
    sendBatch(header, out);
}

}  // namespace

Vu1Regs& vu1() { return g_vu; }
void vu1SetKick(Vu1KickFn fn) { g_kick = fn; }
std::uint64_t vu1Runs() { return g_runs; }
std::uint64_t vu1Unported() { return g_unported; }

void vu1OnMscal(std::uint32_t pc, std::uint32_t top, std::uint32_t itop) {
    if ((pc & 0xFu) != 0u || pc >= 0x80u) {
        ++g_unported;  // direct-address programs (0x90..0x1908 region): next step
        return;
    }
    g_top = top & kQwordMask;
    (void)itop;
    switch (pc / 16u) {
        case 0: runInit(); break;
        case 1: runSetupLit(); break;
        case 2: runLitStrip(); break;
        case 5: runOffsetStrip(); break;
        // 3 ObjectStrip, 4 WorldStrip, 6 SkyStrip, 7 SetupClipped: clipped-program
        // family (race/interior scenes), transliterated later in W3.
        default: ++g_unported; return;
    }
    ++g_runs;
}

}  // namespace rtguest
