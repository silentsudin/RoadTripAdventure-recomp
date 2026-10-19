// Pins W11-29: drawArcRibbon (0x002224f8), authored this pass, and the fixed
// f-register bit-carry through the sinf/cosf chain (defect class 2026-10-19:
// pseudocode is float-typed; the chain must exchange float-register BITS through
// uint32_t params, never value-convert).
//
// drawArcRibbon facts pinned (oracle MIPS decode, generated/sub_002224F8):
// - matrix product into 4 frame quads, then two 21-iteration chains; each does
//   the column-major idiom (vmulax/vmadday/vmaddaz/vmaddw -> vf9) on the point
//   quad at 0x29d310 and on (0x29d320,d324,d328,d32c), storing .x/.y/.z
//   (qmfc2 + prot3w rotate-left-1) into the vertex chains;
// - loop 1: point (0.5, DAT_00332ce4, d318), arc (sin*+0.5+f9, cos*+f8, d328);
//   loop 2: point (-0.5, DAT_00332cec, d318), arc ((sin*-0.5)-f9, cos*+f8, d328
//   = uVar10 = 29.0 written before loop 2 when param_2 == 1);
// - chain stride 0x78; flag words at chain+0x38/+0x74 zeroed each iteration.
#include "guest/guest_shim.h"

#include <cassert>
#include <cmath>
#include <cstring>

namespace guest {
void drawArcRibbon(uint32_t q0 = 0, long q1 = 0, ...);
uint32_t sinf(uint32_t q0 = 0, ...);
uint32_t cosf(uint32_t q0 = 0, ...);
uint32_t sqrtf(uint32_t q0 = 0, ...);
uint32_t atanf(uint32_t q0 = 0, ...);
uint32_t atan2f(uint32_t q0 = 0, uint32_t q1 = 0, ...);
uint32_t fabsf(uint32_t q0 = 0, ...);
// W12 kernel stubs (kernel_stubs.json) are not implemented yet; this pin is the
// first test whose link closure reaches them. Weak shims until W12 lands.
__attribute__((weak)) uint32_t FlushCache(uint32_t, ...) { return 0; }
__attribute__((weak)) uint32_t SetVSyncFlag(uint32_t, ...) { return 0; }
}

static void setf(uint32_t a, float f) { *(float*)rtguest::G(a) = f; }
static float getf(uint32_t a) { return *(float*)rtguest::G(a); }

int main() {
  std::memset(rtguest::kRam, 0, sizeof(rtguest::kRam));

  // --- sinf/cosf fast path: exact fdlibm polynomials, bit-for-bit ---
  // sinf(0.5f): k_sinf fast path (|x|bits < 0x3f490fd9).
  float x = 0.5f;
  float z = x * x, r = z * x;
  float p = z * (z * (z * (z * 1.589691e-10f + -2.505076e-08f) + 2.7557314e-06f) +
                 -0.0001984127f) + 0.008333334f;
  float exp_sin = x + r * (z * p + -0.16666667f);
  assert(rtguest::fbits(guest::sinf(rtguest::f2bits(x))) == exp_sin);

  // cosf(0.5f): k_cosf first band.
  float pc = z * (z * (z * (z * (z * (z * -1.1359648e-11f + 2.0875723e-09f) +
                                 -2.7557314e-07f) + 2.4801588e-05f) + -0.0013888889f) +
                  0.041666668f);
  float exp_cos = 1.0f - (z * 0.5f - (z * pc - x * 0.0f));
  assert(rtguest::fbits(guest::cosf(rtguest::f2bits(x))) == exp_cos);

  // rem_pio2f band (|x| > 2.36): quadrant path runs and lands near truth.
  float s3 = rtguest::fbits(guest::sinf(rtguest::f2bits(3.0f)));
  assert(std::fabs(s3 - 0.1411200f) < 2e-6f);
  float c3 = rtguest::fbits(guest::cosf(rtguest::f2bits(3.0f)));
  assert(std::fabs(c3 + 0.9899925f) < 2e-6f);

  // --- drawArcRibbon with identity * identity ---
  const uint32_t mtx = 0x00400000;
  for (int pass = 0; pass < 2; pass++) {          // A at mtx, B at mtx+0x40
    for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
        setf(mtx + pass * 0x40 + (uint32_t)(i * 4 + j) * 4, i == j ? 1.0f : 0.0f);
  }
  setf(0x00332ce4, 3.25f);   // loop1 point .y
  setf(0x00332ce8, 0.0f);    // loop1 angle scale -> angle 0
  setf(0x00332cec, -1.0f);   // loop2 point .y
  setf(0x00332cf0, 0.0f);    // loop2 angle scale
  setf(0x00332cd0, 10.0f);   // f6 (arc radius) for param_2 == 1
  setf(0x00332cd4, 0.0f);    // f8
  setf(0x0029d318, 99.0f);   // point .z (ram)
  setf(0x0029d328, 42.0f);   // arc .z during loop 1 (prev-frame value)
  setf(0x0029d32c, 1.0f);    // .w
  // vif globals: packet write cursor (else fatal spins); buffer past the asserted ranges
  *(uint32_t*)rtguest::G(0x003356EC) = 0x0029d500;  // vifPacketPtr
  *(uint32_t*)rtguest::G(0x003356F4) = 0;           // vifPacketSecond

  guest::drawArcRibbon(mtx, 1);

  for (int i = 0; i < 0x15; i++) {
    uint32_t o = (uint32_t)i * 0x78;
    // loop1 first chain: identity * (0.5, 3.25, 99.0)
    assert(getf(0x00299e54 + o) == 0.5f);
    assert(getf(0x00299e58 + o) == 3.25f);
    assert(getf(0x00299e5c + o) == 99.0f);
    // loop1 arc: sin(0)*10 + 0.5 + 6.5 = 7.0, cos(0)*10 + 0 = 10.0, z = d328 = 42.0
    assert(getf(0x00299e90 + o) == 7.0f);
    assert(getf(0x00299e94 + o) == 10.0f);
    assert(getf(0x00299e98 + o) == 42.0f);
    // loop2 first chain: (-0.5, -1.0, 99.0)
    assert(getf(0x00299458 + o) == -0.5f);
    assert(getf(0x0029945c + o) == -1.0f);
    assert(getf(0x00299460 + o) == 99.0f);
    // loop2 arc: (0*10 - 0.5) - 6.5 = -7.0, 10.0, z = uVar10 = 29.0
    assert(getf(0x00299494 + o) == -7.0f);
    assert(getf(0x00299498 + o) == 10.0f);
    assert(getf(0x0029949c + o) == 29.0f);
    // flag words zeroed
    assert(*(uint32_t*)rtguest::G(0x00299e8c + o) == 0);
    assert(*(uint32_t*)rtguest::G(0x00299ec8 + o) == 0);
    assert(*(uint32_t*)rtguest::G(0x00299490 + o) == 0);
    assert(*(uint32_t*)rtguest::G(0x002994cc + o) == 0);
  }
  // uVar10 (29.0 for mode 1) landed in d328 for loop 2
  assert(getf(0x0029d328) == 29.0f);

  // --- libm float-bit cluster (0x274af8/0x274b10 thunks, ieee754 0x2750e0/
  // 0x275690, atanf 0x277740, fabsf 0x2779e0): params/returns carry f-register
  // BITS; value conversions between them are the defect class fixed 2026-10-19.
  assert(guest::fabsf(0xbf800000u) == 0x3f800000u);  // mov.s/swc1/and 0x7fffffff/mtc1
  // sqrtf chain: digit-by-digit integer sqrt on bits (fdlibm e_sqrtf.c).
  assert(guest::sqrtf(0x40800000u) == 0x40000000u);  // sqrt(4)=2 exact
  assert(guest::sqrtf(0x3f800000u) == 0x3f800000u);  // sqrt(1)=1
  assert(guest::sqrtf(0x3ff00000u) == 0x3fb504f3u);  // sqrt(2) correctly rounded
  assert(guest::sqrtf(0xbf800000u) == 0x7fc00000u);  // negative -> div.s 0/0 qNaN
  assert(guest::sqrtf(0x00400000u) == 0x00400000u);  // |bits|<=0x7fffff: as-is
  // atanf (fdlibm a_tanf band structure): tiny |x| returns x's bits.
  assert(guest::atanf(0x30000000u) == 0x30000000u);
  assert(guest::atanf(0x00400000u) == 0x00400000u);
  // |x| bits > 0x507fffff -> +/-0x3fc90fdb (0x27775x imm pairs).
  assert(guest::atanf(0x7149f2b0u) == 0x3fc90fdbu);
  assert(guest::atanf(0xf149f2b0u) == 0xbfc90fdbu);
  // Table bands: seed 0x331b38/0x331b48 from ELF bytes (PT_LOAD read).
  // hi = {0x3eed6338,0x3f490fda,0x3f7b985e,0x3fc90fda} lo = {0x31ac3769,
  // 0x33222168,0x33140fb4,0x33a22168}; hi[1],lo[1] are fdlibm pio4_1/pio4_2.
  const uint32_t hi[4] = {0x3eed6338u, 0x3f490fdau, 0x3f7b985eu, 0x3fc90fdau};
  const uint32_t lo[4] = {0x31ac3769u, 0x33222168u, 0x33140fb4u, 0x33a22168u};
  for (int k = 0; k < 4; ++k) {
    *(uint32_t*)rtguest::G(0x00331b38 + 4u * k) = hi[k];
    *(uint32_t*)rtguest::G(0x00331b48 + 4u * k) = lo[k];
  }
  // atan(1.0): band s=1, t=(1-1)/(1+1)=0 -> hi[1]+lo[1] = correctly-rounded pi/4.
  assert(guest::atanf(0x3f800000u) == 0x3f22f983u);
  // atan(0.5): band s=0 (0x3f300000 bound), t=(2*0.5-1)/(0.5+2)=0 -> hi[0]+lo[0].
  float h0 = *(float*)&hi[0];
  float l0 = *(float*)&lo[0];
  assert(guest::atanf(0x3f000000u) == rtguest::f2bits(h0 + l0));
  // atan2f thunk -> ieee754_atan2f (f12/f13 sequential float ABI at 0x2750e4):
  assert(guest::atan2f(0x00000000u, 0x00000000u) == 0u);        // hy tiny, u2=0 -> y
  assert(guest::atan2f(0x80000000u, 0x00000000u) == 0x80000000u);  // -0.0 passes through
  assert(guest::atan2f(0x00400000u, 0xbf800000u) == 0x40490fda);  // tiny y, x<0
  assert(guest::atan2f(0x80400000u, 0xbf800000u) == 0xc0490fda);  // -tiny y, x<0
  assert(guest::atan2f(0x30000000u, 0x3f800000u) == 0x30000000u);  // x==1.0 fast path
  assert(guest::atan2f(0x40000000u, 0x00400000u) == 0x3fc90fdb);   // tiny x, y>0
  assert(guest::atan2f(0xc0000000u, 0x00400000u) == 0xbfc90fdb);   // tiny x, y<0
}
