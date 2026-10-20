// Pins the W13 float-param fix: guest callees take f12.. args as uint32 bit patterns and
// compare/multiply them as REAL floats (fn/00244988.c:58 `param_1 == 0.0`, fn/0021c928.c).
// Before the fix a float arg value-converted to an int (0.5 -> 0) and the ==0.0 branch was
// taken (BuildCarouselQuadA wrote 0x80 where the oracle wrote 0x60).
#undef NDEBUG  // asserts must stay live in Release
#include "guest/guest_shim.h"

#include <cassert>
#include <cstring>

namespace guest {
uint32_t BuildCarouselQuadA(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, ...);
uint32_t StoreIndexedPosition(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, ...);
}
#include "guest/gen/w13_float_args.h"

static uint32_t w_(uint32_t a) { return *(uint32_t*)rtguest::G(a); }

int main() {
  std::memset(rtguest::kRam, 0, RT_GUEST_RAM_SIZE);
  constexpr uint32_t q = 0x100000;
  // bit pattern 0x00000000 == 0.0f -> iVar2 = 0x80 on word -1 of the first group (q+0x4c-4)
  guest::BuildCarouselQuadA(0u, q, 0, 0, 0, 0);
  assert(w_(q + 0x48) == 0x80);
  // fractional 0.5f must NOT collapse to 0: the cos branch runs (Ghidra's fVar7 stays 0 -> 0x60)
  guest::BuildCarouselQuadA(rtguest::f2bits(0.5f), q, 0, 0, 0, 0);
  assert(w_(q + 0x48) == 0x60);
  // float-typed argument through the forwarding template carries bits, not a truncated int
  guest::BuildCarouselQuadA(0.5f, q + 0x1000, 0, 0, 0, 0);
  assert(w_(q + 0x1000 + 0x48) == 0x60);

  // StoreIndexedPosition: fractional floats stored as bits; int copies scaled by DAT_00332c24
  *(float*)rtguest::G(0x332c24) = 2.0f;
  guest::StoreIndexedPosition(1.5f, 2.5f, 3.5f, 0, 0);
  assert(w_(0x177ace0) == rtguest::f2bits(1.5f));
  assert(w_(0x177ace4) == rtguest::f2bits(2.5f));
  assert(w_(0x177ace8) == rtguest::f2bits(3.5f));
  assert(w_(0x177acf0) == 3 && w_(0x177acf4) == 5 && w_(0x177acf8) == 7);
  return 0;
}
