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
#include <algorithm>
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
constexpr std::uint32_t kTint = 24;          // tint (LitStrip/SkyStrip dis6 1ae8) / colour scale (OffsetStrip dis5 loop)
constexpr std::uint32_t kWeights = 25;       // cycling weights: offset (dis5) / sky vertex (dis6 1af0)
constexpr std::uint32_t kBatchOffset = 26;   // world offset added to every vertex (dis4 0098: LQ f31, 26(vi0))
constexpr std::uint32_t kSyncTag = 29;       // empty EOP tag kicked between polygon packets (dis4 03b0-03d8)
constexpr std::uint32_t kPolygonPacket = 30; // clipped polygon: GIF tag + 3 quads per vertex (template the game pre-stores)
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
// MSUB acc - a*b (.inl upper 0x0C/0x2D): plus its exact value, which is what the
// sky strip's winding test reads the sign from (dis6 1c18-1c20 OPMULA/OPMSUB pair).
inline float fmsub(float acc, float a, float b, double* exact = nullptr) {
    acc = normIn(acc); a = normIn(a); b = normIn(b);
    const double e = static_cast<double>(acc) - static_cast<double>(a) * static_cast<double>(b);
    if (exact) *exact = e;
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
inline std::uint32_t clipFlags(const float* v, float w) {
    std::uint32_t wBits;
    std::memcpy(&wBits, &w, 4);
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
inline std::uint32_t clipFlags(const float* v, const float* wReg) { return clipFlags(v, wReg[3]); }
// ITOF0 (.inl:1607): integer bits -> float (the clipper re-reads stored RGBA as floats).
inline float itof0(std::uint32_t b) { return static_cast<float>(static_cast<std::int32_t>(b)); }

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
inline Mat copyMat(const float (*vf)[4], int base) {
    Mat m;
    for (int i = 0; i < 4; ++i) std::memcpy(m.col[i], vf[base + i], 16);
    return m;
}
inline std::uint32_t bitsOf(float v) {
    std::uint32_t b;
    std::memcpy(&b, &v, 4);
    return b;
}
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
        loadQ(mem::kFogLit + i, g_vu.vf[9 + i]);  // VF9..11 = VU19..21 (init dis 12e8-12f8: LQ 19/20/21)
    }
    g_vu.vi[14] = 1024;  // register-save stack pointer (IADDIU -1024 at dis 1268: only its low 10 bits reach addressing)
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

// CLIP flag bits of one vertex, and the rotation the clip register shows for three
// consecutive CLIPs (dis4 0188/01b0/0208 chain read as IOR vi1 flags at 03c8).
constexpr std::uint32_t kOutside = 0x3Fu;
constexpr std::uint32_t kPlusX = 0x01u, kMinusX = 0x02u, kPlusY = 0x04u, kMinusY = 0x08u, kPlusZ = 0x10u, kMinusZ = 0x20u;
inline std::uint32_t spread(std::uint32_t bit) { return bit | (bit << 6) | (bit << 12); }
inline std::uint32_t clipWindow(std::uint32_t older, std::uint32_t old, std::uint32_t newest) {
    return ((older << 12) | (old << 6) | newest) & 0x3FFFFu;
}
// The trivial cases (dis4 02f8-0368 plane tests): all three vertices beyond the same
// side (-y, +y, -x, +x, -z), or any beyond +z: nothing of the triangle is drawn.
bool rejected(std::uint32_t window) {
    for (std::uint32_t bit : {kMinusY, kPlusY, kMinusX, kPlusX, kMinusZ})
        if ((window & spread(bit)) == spread(bit)) return true;
    return (window & spread(kPlusZ)) != 0u;
}
// XYZF2.w |= ADC (ILW/ISW pair at dis4 03b0-03d0).
void setAdc(std::uint32_t xyzfAddr) {
    const std::int16_t w = half(xyzfAddr, 3);
    setHalf(xyzfAddr, 3, static_cast<std::uint16_t>(w | static_cast<std::int16_t>(0x8000)));
}

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

// ---- Programs 3/4/6/7: the clipped family -------------------------------------------------
// ObjectStrip (pc 0x30, body 0x03e8), WorldStrip (pc 0x40, body 0x0080), SkyStrip (pc 0x60,
// body 0x1aa0) and SetupClipped (pc 0x70, body 0x1318). A triangle leaving the clip volume
// is rejected (all three beyond one side) or clipped against the six planes and sent as a
// packet of its own (XGKICK at dis4 02c8); either way the strip's closing vertex gets ADC.
// The polygon packet is built in memory at kPolygonPacket and kicked per polygon; the batch
// header goes with the first, an empty EOP tag (dis4 03b0-03d8 rewrites quads 29/30) between
// the rest. Verified against qr0_rare (2840 ObjectStrip, 720 SkyStrip, 40 SetupClipped) and
// qr0_world (8000 WorldStrip) captures.

// A polygon vertex while clipping: clip-space position, colour (float), texture.
struct ClipVertex {
    float pos[4], rgba[4], st[4];
};

struct Clipper {
    Mat screen;           // VF1..4: clip -> screen
    float* fog;           // VF12 (VF8 for the sky): w far, z scale, y max
    std::uint32_t tagY;   // the polygon packet's GIF tag word y (PRIM etc.)
    std::uint32_t tagZ;   // REGS
    std::uint32_t& header;  // the batch header still to send (0 once sent)

    // The polygon lives in VU memory, 3 quads per vertex (position, colour, st), closed
    // by a copy of its first vertex; passes ping-pong between two buffers.
    static constexpr std::uint32_t kBufferA = 61, kBufferB = 101;
    void read(std::uint32_t addr, ClipVertex& v) const {
        loadQ(addr, v.pos); loadQ(addr + 1, v.rgba); loadQ(addr + 2, v.st);
    }
    void write(std::uint32_t addr, const ClipVertex& v) const {
        storeQ(addr, v.pos); storeQ(addr + 1, v.rgba); storeQ(addr + 2, v.st);
    }

    // One Sutherland-Hodgman pass of the closed polygon at src (`edges` edges) against
    // plane `component` (0 x, 1 y, 2 z) on the `sign` side, into dst: inside is
    // sign*v[component] <= w, as CLIP judges it (ADD x,0 normalises every lane on read).
    // The intersection is the edge a->b at t = |da / (db - da)| with d = v[component] - sign*w.
    std::uint32_t pass(std::uint32_t src, std::uint32_t dst, std::uint32_t edges, int component, float sign,
                       std::uint32_t bit) const {
        std::uint32_t out = dst, count = 0;
        for (std::uint32_t e = 0; e < edges; ++e) {
            ClipVertex a, b;
            read(src + 3 * e, a);
            for (int l = 0; l < 4; ++l) {
                a.pos[l] = fadd(a.pos[l], 0.0f);
                a.rgba[l] = fadd(a.rgba[l], 0.0f);
                a.st[l] = fadd(a.st[l], 0.0f);
            }
            read(src + 3 * e + 3, b);
            const bool aOut = (clipFlags(a.pos, a.pos[3]) & bit) != 0u;
            const bool bOut = (clipFlags(b.pos, b.pos[3]) & bit) != 0u;
            if (!aOut) {
                write(out, a);
                out += 3;
                ++count;
            }
            if (aOut != bOut) {
                ClipVertex r;
                intersect(a, b, component, sign, r);
                write(out, r);
                out += 3;
                ++count;
            }
        }
        ClipVertex first;
        read(dst, first);  // closed again
        write(out, first);
        return count;
    }

    static void intersect(const ClipVertex& a, const ClipVertex& b, int c, float sign, ClipVertex& r) {
        const float da = fsub(a.pos[c], fmul(a.pos[3], sign));
        const float db = fsub(b.pos[c], fmul(b.pos[3], sign));
        const float q = fdiv(da, fsub(db, da));
        const float t = std::fabs(normIn(fadd(0.0f, q)));  // ADD x,0 + the operand normalisation
        for (int l = 0; l < 4; ++l) {
            r.pos[l] = fadd(fmul(fsub(b.pos[l], a.pos[l]), t), a.pos[l]);
            r.rgba[l] = fadd(fmul(fsub(b.rgba[l], a.rgba[l]), t), a.rgba[l]);
            r.st[l] = fadd(fmul(fsub(b.st[l], a.st[l]), t), a.st[l]);
        }
    }

    // Clips triangle (v0, v1, v2) against -z, +z, -x, +x, -y, +y (three BAL passes at
    // dis4 0380/0390/03a0 -> 0x1040/0x720/0x1150, each both planes of one axis pair... in
    // HLE-verified order) and sends what is left as one packet. An empty result after any
    // plane but the first ends it (the first can't empty a triangle that wasn't rejected).
    void clipAndSend(const ClipVertex& v0, const ClipVertex& v1, const ClipVertex& v2) {
        write(kBufferA, v0);
        write(kBufferA + 3, v1);
        write(kBufferA + 6, v2);
        write(kBufferA + 9, v0);
        struct Plane { int component; float sign; std::uint32_t bit; };
        static constexpr Plane kPlanes[] = {{2, -1.0f, kMinusZ}, {2, 1.0f, kPlusZ}, {0, -1.0f, kMinusX},
                                            {0, 1.0f, kPlusX},   {1, -1.0f, kMinusY}, {1, 1.0f, kPlusY}};
        std::uint32_t count = 3, src = kBufferA, dst = kBufferB;
        for (const Plane& p : kPlanes) {
            count = pass(src, dst, count, p.component, p.sign, p.bit);
            if (count == 0u && p.bit != kMinusZ) return;
            std::swap(src, dst);
        }
        send(count);  // the result is in buffer A
    }

    void send(std::uint32_t count) const {
        // Wait for the previous polygon's packet (the empty EOP tag at quad 29), or send
        // the batch header first if it hasn't gone yet (dis4 03b0-03d0 rewrites that quad).
        setHalf(mem::kSyncTag, 0, 0x8000u);
        kickPacket(header != 0u ? header : mem::kSyncTag);

        float tag[4];
        loadQ(mem::kPolygonPacket, tag);
        setBits(tag[1], tagY);
        setBits(tag[2], tagZ);
        storeQ(mem::kPolygonPacket, tag);
        setHalf(mem::kPolygonPacket, 0, static_cast<std::uint16_t>(0x8000u | count));
        for (std::uint32_t j = 0; j < count; ++j) {
            ClipVertex v;
            read(kBufferA + 3 * j, v);
            float hpos[4];
            transform(hpos, screen, v.pos);
            const float fogBase = fsub(fog[3], hpos[3]);
            const float q = fdiv(1.0f, hpos[3]);
            float f = fmul(fogBase, fog[2]);
            f = fmax(f, 0.0f);
            f = fmin(f, fog[1]);
            float stq[4], rgba[4], xyzf[4];
            stq[0] = fmul(v.st[0], q);
            stq[1] = fmul(v.st[1], q);
            stq[2] = fmul(1.0f, q);
            stq[3] = fogBase;
            for (int l = 0; l < 4; ++l) setBits(rgba[l], ftoi(v.rgba[l], 1.0));
            for (int l = 0; l < 3; ++l) hpos[l] = fmul(hpos[l], q);
            hpos[3] = f;
            for (int l = 0; l < 4; ++l) setBits(xyzf[l], ftoi(hpos[l], 16.0));
            const std::uint32_t at = mem::kPolygonPacket + 1 + 3 * j;
            storeQ(at, stq);
            storeQ(at + 1, rgba);
            storeQ(at + 2, xyzf);
        }
        kickPacket(mem::kPolygonPacket);
        if (header != 0u) {
            setHalf(header, 0, 0);
            header = 0u;
        }
    }
};

// The colour of a vertex as the clipper gets it: the RGBA just written (ITOF0 chain),
// back to float.
void rgbaFloat(std::uint32_t addr, float* r) {
    float v[4];
    loadQ(addr, v);
    for (int l = 0; l < 4; ++l) {
        std::uint32_t b;
        std::memcpy(&b, &v[l], 4);
        r[l] = itof0(b);
    }
}

// Fog from a parameter register: (far - w) * scale, at least 0 (before the caps).
inline float fogRaw(float w, const float* p) { return fmax(fmul(fsub(p[3], w), p[2]), 0.0f); }

// Programs 3 and 4 (the same loop, dis4 0140-0270 / dis3 0468-...): ObjectStrip takes
// local positions through the object's rotation (VF5..7) and origin (TOP+1), WorldStrip
// world positions plus the batch offset VU26. Colour: three sets A, B, C blended by the
// weights VF9. Fog set VF10, or VF11 when the tag's y word has bit 1 (dis4 00b0).
void runColourSetStrip(bool object) {
    std::uint32_t header = half(headerAddr(), 0) != 0 ? headerAddr() : 0u;
    const std::uint32_t count = static_cast<std::uint32_t>(half(g_top, 0)) & 0x7FFFu;
    const Mat screen = copyMat(g_vu.vf, 1), clipM = copyMat(g_vu.vf, 13);
    const float* weights = g_vu.vf[9];  // x, y, z: weights of colour sets A, B, C
    const float* fogB = g_vu.vf[11];

    float fog[4];
    for (int l = 0; l < 4; ++l) fog[l] = fadd(g_vu.vf[10][l], 0.0f);
    if (half(g_top, 1) & 2) std::memcpy(fog, g_vu.vf[11], 16);
    std::memcpy(g_vu.vf[12], fog, 16);

    float tag[4];
    loadQ(g_top, tag);
    const std::uint32_t in = g_top + (object ? 2u : 1u);
    const std::uint32_t out = in + 250u;  // fixed buffer window (dis4 00d8: IADDIU vi2, vi3, 250)
    if (count == 0u) {  // the microcode copies the tag and sends the empty batch (dis4 00f0/0290/0298)
        storeQ(out, tag);
        sendBatch(header, out);
        return;
    }
    constexpr std::uint32_t kStride = 5;
    storeQ(out, tag);
    std::uint32_t tagYw;
    {
        const float v = fadd(0.0f, tag[3]);
        std::memcpy(&tagYw, &v, 4);
    }

    float offset[4];
    loadQ(mem::kBatchOffset, offset);
    Mat rotation;
    std::memcpy(rotation.col[0], g_vu.vf[5], 16);
    std::memcpy(rotation.col[1], g_vu.vf[6], 16);
    std::memcpy(rotation.col[2], g_vu.vf[7], 16);
    for (int l = 0; l < 4; ++l) rotation.col[3][l] = 0.0f;
    if (object) {
        float o[4];
        loadQ(g_top + 1, o);
        for (int l = 0; l < 3; ++l) o[l] = fadd(o[l], offset[l]);
        float origin[4];
        transformPoint(origin, clipM, o);
        std::memcpy(g_vu.vf[8], origin, 16);
        std::memcpy(rotation.col[3], origin, 16);
    }

    Clipper clipper{screen, fog, tagYw, bitsOf(tag[2]), header};
    float stqW;
    std::memcpy(&stqW, &g_vu.vf[27][3], 4);  // VF27.w left over from the last program (the record's w isn't loaded)
    float cpos[3][4] = {{0, 0, 0, 1}, {0, 0, 0, 1}, {}};
    std::uint32_t flags[3] = {0, 0, 0};
    float lastA[4], lastB[4];
    std::memcpy(lastA, g_vu.vf[17], 16);
    std::memcpy(lastB, g_vu.vf[18], 16);
    std::uint32_t lastRgba[4] = {};
    OutsideWindow window;
    for (std::uint32_t k = 0; k < count; ++k) {
        const std::uint32_t rec = in + k * kStride;
        float pos[4], a[4], b[4], c[4], st[4];
        loadQ(rec, pos); loadQ(rec + 1, a); loadQ(rec + 2, b); loadQ(rec + 3, c); loadQ(rec + 4, st);
        float p[4];
        if (object) {
            transformPoint(p, rotation, pos);  // rotation x local + origin (dis3 loop MADD f25, col3)
        } else {
            for (int l = 0; l < 3; ++l) pos[l] = fadd(pos[l], offset[l]);
            transformPoint(p, clipM, pos);
        }
        std::memcpy(cpos[0], cpos[1], 16);
        std::memcpy(cpos[1], cpos[2], 16);
        std::memcpy(cpos[2], p, 16);
        flags[0] = flags[1];
        flags[1] = flags[2];
        flags[2] = clipFlags(p, p[3]);

        float hpos[4];
        transform(hpos, screen, p);
        g_vu.q = fdiv(1.0f, hpos[3]);  // dis4 0180 DIV
        const float q = g_vu.q;
        const float f0 = fogRaw(hpos[3], fog);
        const float f = fmin(f0, fog[1]);

        // Colour: three sets blended by the weights (dis4 01b8-0218 chain); A.w carries
        // B.x, C.w the alpha base (offset.w) + st.z.
        float A[4] = {a[0], a[1], a[2], b[0]};
        float C[4] = {c[0], c[1], c[2], fadd(offset[3], st[2])};
        float acc[4];
        for (int l = 0; l < 4; ++l) acc[l] = fmul(A[l], weights[0]);
        acc[1] = fmadd(acc[1], b[1], weights[1]);
        acc[2] = fmadd(acc[2], b[2], weights[1]);
        acc[0] = fmadd(acc[0], A[0], weights[1]);
        acc[3] = fmadd(acc[3], A[3], weights[1]);
        float colour[4];
        for (int l = 0; l < 4; ++l) colour[l] = fmadd(acc[l], C[l], weights[2]);
        if (object) {
            colour[3] = fmul(fmin(f0, fog[0]), colour[3]);
        } else {
            const float alphaFog = fmin(fogRaw(hpos[3], fogB), fogB[0]);
            colour[3] = fmin(colour[3], 1.0f);
            for (int l = 0; l < 3; ++l) colour[l] = fmin(colour[l], fog[1]);
            colour[3] = fmul(alphaFog, colour[3]);
        }

        float stq[4], rgba[4], xyzf[4];
        stq[0] = fmul(st[0], q);
        stq[1] = fmul(st[1], q);
        stq[2] = fmul(1.0f, q);
        stq[3] = stqW;
        for (int l = 0; l < 3; ++l) hpos[l] = fmul(hpos[l], q);
        hpos[3] = f;
        for (int l = 0; l < 4; ++l) {
            setBits(xyzf[l], ftoi(hpos[l], 16.0));
            setBits(rgba[l], lastRgba[l] = ftoi(colour[l], 1.0));
        }
        const std::uint32_t at = out + 1 + 3 * k;
        storeQ(at, stq);
        storeQ(at + 2, xyzf);
        storeQ(at + 1, rgba);
        std::memcpy(lastA, A, 16);
        lastB[0] = b[0]; lastB[1] = b[1]; lastB[2] = b[2];

        const std::int16_t outsideSum = window.push((flags[2] & kOutside) ? 1 : 0);
        if (outsideSum > 0) {
            if (!rejected(clipWindow(flags[0], flags[1], flags[2]))) {
                const std::uint32_t base = in + (k - 2) * kStride + 4;  // st of vertex k-2
                ClipVertex cv[3];
                std::memcpy(cv[0].pos, cpos[0], 16);
                std::memcpy(cv[1].pos, cpos[1], 16);
                std::memcpy(cv[2].pos, cpos[2], 16);
                rgbaFloat(at - 5, cv[0].rgba);
                rgbaFloat(at - 2, cv[1].rgba);
                rgbaFloat(at + 1, cv[2].rgba);
                loadQ(base, cv[0].st);
                loadQ(base + kStride, cv[1].st);
                loadQ(base + 2 * kStride, cv[2].st);
                clipper.clipAndSend(cv[0], cv[1], cv[2]);
            }
            setAdc(at + 2);  // dis4 03b0-03d0: ILW/ISW patch three vertices back
        }
    }
    std::memcpy(g_vu.vf[17], lastA, 16);
    std::memcpy(g_vu.vf[18], lastB, 16);
    for (int l = 0; l < 4; ++l) setBits(g_vu.vf[28][l], lastRgba[l]);  // VF28 = last RGBA bits (carry)
    sendBatch(header, out);  // dis4 0280-02a0: XGKICK vi13 (if kept) + XGKICK vi2
}

// ---- Program 7: SetupClipped (slot pc 0x70; body 0x1318-0x1418, dis sc) ---------------------
// world->clip = clip x world into VF13..16, the clip->screen matrix as is, and the
// rotation light x world (VF5..7: the first column chained w, z, y, x; VU20-23 hold the
// light matrix while VF9..12 take the colours).
void runSetupClipped() {
    Mat world, clipM, light;
    for (int i = 0; i < 4; ++i) {
        loadQ(mem::kWorld + i, world.col[i]);
        loadQ(mem::kClip + i, clipM.col[i]);
    }
    for (int i = 0; i < 3; ++i) loadQ(mem::kLightDirs + i, light.col[i]);
    for (int l = 0; l < 4; ++l) light.col[3][l] = l == 3 ? 1.0f : 0.0f;
    for (int i = 0; i < 4; ++i) {
        float r[4];
        transform(r, clipM, world.col[i]);
        std::memcpy(g_vu.vf[13 + i], r, 16);
    }
    {  // the first column is chained in the opposite order: w, z, y, x (dis sc 13a0-13b8 reversed)
        float acc[4];
        for (int l = 0; l < 4; ++l) acc[l] = fmul(light.col[3][l], world.col[0][3]);
        for (int l = 0; l < 4; ++l) acc[l] = fmadd(acc[l], light.col[2][l], world.col[0][2]);
        for (int l = 0; l < 4; ++l) acc[l] = fmadd(acc[l], light.col[1][l], world.col[0][1]);
        for (int l = 0; l < 4; ++l) acc[l] = fmadd(acc[l], light.col[0][l], world.col[0][0]);
        std::memcpy(g_vu.vf[5], acc, 16);
    }
    for (int i = 1; i < 3; ++i) {
        float r[4];
        transform(r, light, world.col[i]);
        std::memcpy(g_vu.vf[5 + i], r, 16);
    }
    for (int i = 0; i < 4; ++i) {
        loadQ(mem::kClipToScreen + i, g_vu.vf[1 + i]);
        loadQ(mem::kLightColours + i, g_vu.vf[9 + i]);
    }
    loadQ(mem::kFogLit, g_vu.vf[8]);
    std::memcpy(g_vu.vf[17], world.col[0], 16);  // temporaries that stay behind (dis sc 1318/1340)
    std::memcpy(g_vu.vf[18], world.col[1], 16);
    g_vu.vf[27][3] = clipM.col[2][3];
    g_vu.vf[28][3] = clipM.col[3][3];
    for (int l = 0; l < 2; ++l) g_vu.vf[28][l] = 0.0f;  // dis sc 1400: SQ.xy f0, 28(vi0)
    setHalf(571 + mem::kHeaderOffset, 0, 0);            // the other buffer's header marked sent (dis sc 1418)
    g_vu.vi[14] = 1024;
}

// ---- Program 6: SkyStrip (slot pc 0x60; body 0x1aa0-..., dis6) ------------------------------
// TOP+1: y word = the clipped polygons' GIF tag word. Vertex records of 3 quads from
// TOP+2: position, colour, (s, t, q scale). Each vertex moves along VU6 by one of four
// weights (VU25, cycled per vertex); colour x tint VU24; triangles facing the wrong way
// for their place in the strip are culled by the cross product's sign (OPMULA/OPMSUB).
void runSkyStrip() {
    std::uint32_t header = half(headerAddr(), 0) != 0 ? headerAddr() : 0u;
    const std::uint32_t count = static_cast<std::uint32_t>(half(g_top, 0)) & 0x7FFFu;
    const Mat screen = copyMat(g_vu.vf, 1), clipM = copyMat(g_vu.vf, 13);
    float fog[4];
    std::memcpy(fog, g_vu.vf[8], 16);
    float tint[4], weights[4], axis[4];
    loadQ(mem::kTint, tint);
    loadQ(mem::kWeights, weights);
    loadQ(mem::kCamera + 2, axis);  // VU6 (dis6 1af8)

    const std::uint32_t in = g_top + 2;
    const std::uint32_t out = in + 4 * count;  // dis6 1b08-1b18: vi doubling
    float tag[4];
    loadQ(g_top, tag);
    if (count == 0u) {
        storeQ(out, tag);
        sendBatch(header, out);
        return;
    }
    storeQ(out, tag);
    constexpr std::uint32_t kStride = 3;

    std::uint32_t tagYw;
    {
        float o1[4];
        loadQ(g_top + 1, o1);
        std::memcpy(&tagYw, &o1[1], 4);
    }
    Clipper clipper{screen, fog, tagYw, bitsOf(tag[2]), header};
    bool clipperUsed = false;
    float cpos[3][4] = {{0, 0, 0, 1}, {0, 0, 0, 1}, {}};
    std::uint32_t flags[3] = {0, 0, 0};
    float prevEdge[4], prevScreen[4];
    std::memcpy(prevEdge, g_vu.vf[17], 16);
    std::memcpy(prevScreen, g_vu.vf[18], 16);
    std::uint32_t lastXyzf[4] = {};
    OutsideWindow window;
    for (std::uint32_t k = 0; k < count; ++k) {
        const std::uint32_t rec = in + k * kStride;
        float pos[4], colour[4], st[4];
        loadQ(rec, pos); loadQ(rec + 1, colour); loadQ(rec + 2, st);

        float p[4];  // axis*weight, then the clip matrix's columns x pos (dis6 1b18/1b20-1b78)
        for (int l = 0; l < 4; ++l) p[l] = fmul(axis[l], weights[0]);
        for (int l = 0; l < 4; ++l) p[l] = fmadd(p[l], clipM.col[0][l], pos[0]);
        for (int l = 0; l < 4; ++l) p[l] = fmadd(p[l], clipM.col[1][l], pos[1]);
        for (int l = 0; l < 4; ++l) p[l] = fmadd(p[l], clipM.col[2][l], pos[2]);
        for (int l = 0; l < 4; ++l) p[l] = fmadd(p[l], clipM.col[3][l], 1.0f);
        const float w0 = weights[0];  // MR32 cycle (dis6 1b70)
        weights[0] = weights[1];
        weights[1] = weights[2];
        weights[2] = weights[3];
        weights[3] = w0;
        std::memcpy(cpos[0], cpos[1], 16);
        std::memcpy(cpos[1], cpos[2], 16);
        std::memcpy(cpos[2], p, 16);
        flags[0] = flags[1];
        flags[1] = flags[2];
        flags[2] = clipFlags(p, p[3]);

        float hpos[4];
        transform(hpos, screen, p);
        g_vu.q = fdiv(1.0f, hpos[3]);
        const float q = g_vu.q;
        const float f0 = fogRaw(hpos[3], fog);
        const float f = fmin(f0, fog[1]);

        float stq[4], rgba[4], xyzf[4];
        for (int l = 0; l < 3; ++l) {
            stq[l] = fmul(st[l], q);  // sky takes st.z x q too (dis6 MUL.xyz f27)
            hpos[l] = fmul(hpos[l], q);
        }
        stq[3] = st[3];
        hpos[3] = f;
        for (int l = 0; l < 3; ++l) rgba[l] = fmul(colour[l], tint[l]);
        rgba[3] = fmul(tint[3], fmin(f0, fog[0]));

        // Winding: the screen-space edge from the previous vertex crossed with the edge
        // before it (dis6 1bf8 SUB.xyz / 1c18-1c20 OPMULA+OPMSUB); z's sign must alternate.
        float edge[4];
        for (int l = 0; l < 3; ++l) edge[l] = fsub(hpos[l], prevScreen[l]);
        const float crossAcc = fmul(edge[0], prevEdge[1]);
        double crossExact = 0.0;
        fmsub(crossAcc, prevEdge[0], edge[1], &crossExact);
        const bool negative = std::signbit(crossExact);
        const bool culled = negative != ((k & 1u) == 0u);
        for (int l = 0; l < 3; ++l) {
            prevEdge[l] = edge[l];
            prevScreen[l] = hpos[l];
        }

        for (int l = 0; l < 4; ++l) {
            setBits(rgba[l], ftoi(rgba[l] , 1.0));
            setBits(xyzf[l], lastXyzf[l] = ftoi(hpos[l], 16.0));
        }
        const std::uint32_t at = out + 1 + 3 * k;
        storeQ(at, stq);
        storeQ(at + 2, xyzf);
        storeQ(at + 1, rgba);

        if (window.push((flags[2] & kOutside) ? 1 : 0) > 0) {
            if (!rejected(clipWindow(flags[0], flags[1], flags[2]))) {
                clipperUsed = true;
                const std::uint32_t base = in + (k - 2) * kStride + 2;
                ClipVertex cv[3];
                std::memcpy(cv[0].pos, cpos[0], 16);
                std::memcpy(cv[1].pos, cpos[1], 16);
                std::memcpy(cv[2].pos, cpos[2], 16);
                rgbaFloat(at - 5, cv[0].rgba);
                rgbaFloat(at - 2, cv[1].rgba);
                rgbaFloat(at + 1, cv[2].rgba);
                loadQ(base, cv[0].st);
                loadQ(base + kStride, cv[1].st);
                loadQ(base + 2 * kStride, cv[2].st);
                clipper.clipAndSend(cv[0], cv[1], cv[2]);
            }
            setAdc(at + 2);
        } else if (culled) {
            setAdc(at + 2);
        }
    }
    if (clipperUsed) std::memcpy(g_vu.vf[12], fog, 16);  // the clipper's VF12 load only in its branch
    std::memcpy(g_vu.vf[17], prevEdge, 16);
    std::memcpy(g_vu.vf[18], prevScreen, 16);
    for (int l = 0; l < 4; ++l) setBits(g_vu.vf[28][l], lastXyzf[l]);
    float last[4];
    loadQ(in + (count - 1) * kStride + 2, last);
    g_vu.vf[27][3] = last[3];  // VF27.w = the last record's st.w (the colour strips read it back)
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
        case 3: runColourSetStrip(true); break;   // ObjectStrip
        case 4: runColourSetStrip(false); break;  // WorldStrip
        case 5: runOffsetStrip(); break;
        case 6: runSkyStrip(); break;
        case 7: runSetupClipped(); break;
        default: ++g_unported; return;  // direct-address programs (0x90..0x1908): next step
    }
    ++g_runs;
}

}  // namespace rtguest
