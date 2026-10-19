// G2 pins for W11-09. Expected values computed from the pseudocode with exact
// integer/bit arithmetic (build/scratch/port/P09/pins.py):
//  - fpDoubleToInt 0x002866c8 (transcribed line-by-line in pins.py)
//  - MakeVifCall15xGif 0x00223810 (direct bit expression)
//  - __divdi3 0x0028d210 (libgcc algorithm: magnitudes, unsigned divide, sign XOR;
//    INT64_MIN/-1 yields 0x8000000000000000 — the negation wraps to the same bits)
//  - __moddi3 0x0028d8a8 authored TU (sign follows dividend; also covered by
//    test_moddi3.cpp, one cross-pin here for the package gate)
#include "../../src/native/guest/guest_shim.h"

#include <cassert>
#include <cstdint>
#include <cstring>

namespace guest {
uint64_t MakeVifCall15xGif(uint32_t, ...);
uint32_t fpDoubleToInt(uint64_t, ...);
uint64_t __divdi3(int64_t, int64_t, ...);
uint64_t __moddi3(uint64_t, uint64_t);
}

// The libgcc div/mod walks a 256-byte MSB-position table at 0x3327f0/0x3328f0;
// verified byte-for-byte against the ELF: t[0]=0, t[i]=bit_length(i) (no game data).
static void seedPosTable(uint32_t base) {
  auto* t = reinterpret_cast<uint8_t*>(rtguest::G(base));
  t[0] = 0;
  for (int i = 1; i < 256; ++i) t[i] = (uint8_t)(32 - __builtin_clz((unsigned)i));
}

int main() {
  std::memset(rtguest::kRam, 0, RT_GUEST_RAM_SIZE);
  seedPosTable(0x3327f0);
  seedPosTable(0x3328f0);

  // 00223810 MakeVifCall15xGif: idx -> (idx*10+0x1be4) | 0x19308000 | (idx*10+0x1bec)<<0x25 | 0x2010000540000000
  assert(guest::MakeVifCall15xGif(0) == 0x20137D8559309BE4ull);
  assert(guest::MakeVifCall15xGif(1) == 0x20137EC559309BEEull);
  assert(guest::MakeVifCall15xGif(3) == 0x2013814559309C02ull);
  assert(guest::MakeVifCall15xGif(0x15) == 0x201397C559309CB6ull);

  // 002866c8 fpDoubleToInt: raw double bits -> int (from pins.py transcription)
  assert(guest::fpDoubleToInt(0x4000000000000000ull) == 2);        // 2.0
  assert(guest::fpDoubleToInt(0x3FF8000000000000ull) == 1);         // 1.5 -> 1 (drops the .5)
  assert(guest::fpDoubleToInt(0x0000000000000001ull) == 0);         // exp < -0x35 -> 0
  assert(guest::fpDoubleToInt(0x4400000000000000ull) == 9999);      // exp > 0xc -> 9999
  assert(guest::fpDoubleToInt(0x3F1E76DCB8D40000ull) == 0);         // denormal-ish tiny -> 0
  assert(guest::fpDoubleToInt(0x41B3C4FD5DC00000ull) == 331677022); // left-shift path (large exp)

  // 0028d210 __divdi3: truncating signed division
  assert((int64_t)guest::__divdi3(100, 7) == 14);
  assert((int64_t)guest::__divdi3(-100, 7) == -14);
  assert((int64_t)guest::__divdi3(7, -2) == -3);
  assert((int64_t)guest::__divdi3(1LL << 40, 3) == 366503875925LL);
  assert((int64_t)guest::__divdi3(1LL << 40, -3) == -366503875925LL);
  assert(guest::__divdi3(-9223372036854775807LL - 1, -1) == 0x8000000000000000ull);
  assert((int64_t)guest::__divdi3(-9223372036854775807LL - 1, 2) == (-4611686018427387904LL));
  assert((int64_t)guest::__divdi3(0x123456789ABCDEF0LL, 0x5A5A5A) == 221531244874LL);
  assert((int64_t)guest::__divdi3(-0x123456789ABCDEF0LL, 0x5A5A5A) == -221531244874LL);
  assert((int64_t)guest::__divdi3(0, 12345) == 0);
  assert((int64_t)guest::__divdi3(999999999999LL, -1000000) == -999999LL);

  // 0028d8a8 __moddi3: sign follows the dividend
  assert((int64_t)guest::__moddi3((uint64_t)7, (uint64_t)3) == 1);
  assert((int64_t)guest::__moddi3((uint64_t)-7, (uint64_t)3) == -1);
  assert((int64_t)guest::__moddi3((uint64_t)0x123456789ABCDEF0LL, (uint64_t)0x5A5A5A) ==
         (int64_t)(0x123456789ABCDEF0LL % 0x5A5A5A));
  return 0;
}
