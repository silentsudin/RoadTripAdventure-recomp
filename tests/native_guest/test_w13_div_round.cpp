// W13-DIV pins: rtguest::div_s = EE DIV.S (round to nearest, x/0 = +-MAX), the
// oracle's ps2_fpu_div_s. Expected bits: buildCamera's frustum corners in the
// attract oracle EERM pin (/tmp/eerm_p1305pin.bin, cameraBlocks+0x100 =
// 0x177e7d0..), inputs from the ELF table 0x29cf40 and fov 10000.0.
#undef NDEBUG  // asserts must stay live in Release
#include "../../src/native/guest/guest_fpu.h"
#include <cassert>
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <cstring>

using rtguest::div_s;
static uint32_t B(float f) { uint32_t b; std::memcpy(&b, &f, 4); return b; }
static float F(uint32_t b) { float f; std::memcpy(&f, &b, 4); return f; }

static void pins() {
  const float den = F(0x4592dfffu);  // 10000.0 * 0.47 (DAT_00332c80) in RZ mul
  assert(B(div_s(-480.f, 10000.f)) == 0xbd449ba6u);   // 0x177e7d0 x (RZ '/' gave ...a5)
  assert(B(div_s(-168.f, den)) == 0xbd126905u);       // 0x177e7d4 y (RZ gave ...04)
  assert(B(div_s(78400.f, den)) == 0x41857263u);      // 0x177e7f4 y (RZ gave ...62)
  assert(B(div_s(224000.f, 10000.f)) == 0x41b33333u); // 0x177e7f0 x, exact either way
  assert(B(div_s(1.f, 0.f)) == 0x7f7fffffu && B(div_s(-1.f, 0.f)) == 0xff7fffffu);
  assert(B(div_s(1.f, -0.f)) == 0xff7fffffu && B(div_s(0.f, 0.f)) == 0x7f7fffffu);
  assert(B(div_s(0.f, -3.f)) == 0x80000000u && B(div_s(F(1u), 2.f)) == 0u);  // denormal = 0
  assert(B(div_s(F(0x7f7fffffu), 0.5f)) == 0x7f7fffffu);                     // overflow clamps
  assert(B(div_s(F(0x00800000u), 4.f)) == 0u);                                // underflow flushes
  assert(B(div_s(6.f, 3.f)) == 0x40000000u);
}

// Nearest: no neighbour float has a smaller exact residual |a - q*b| (exact in double).
static void fuzz() {
  uint32_t x = 0x12345678u;
  auto rnd = [&] { x ^= x << 13; x ^= x >> 17; x ^= x << 5; return x; };
  for (int i = 0; i < 200000; ++i) {
    const float a = F((rnd() & 0x807fffffu) | ((100u + rnd() % 56u) << 23));
    const float b = F((rnd() & 0x807fffffu) | ((100u + rnd() % 56u) << 23));
    const float q = div_s(a, b);
    const double r = std::fabs((double)a - (double)q * b);
    assert(r <= std::fabs((double)a - (double)std::nextafterf(q, INFINITY) * b));
    assert(r <= std::fabs((double)a - (double)std::nextafterf(q, -INFINITY) * b));
  }
}

int main() {
  pins(); fuzz();
  std::fesetround(FE_TOWARDZERO);  // the game thread's mode: same answers
  pins(); fuzz();
  return 0;
}
