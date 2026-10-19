// Pins W11-36: FlagIndexGetNth (0x269bd0) and FlagIndexBitsUpTo (0x269c80)
// param_3==-1 fast path — tail-j 0x24f3b8 with $4/$5 passthrough (the draft
// called the popcount helper with q0=0 defaults; pins prove param_1/param_2
// now reach it).
#include "guest/guest_shim.h"

#include <cassert>
#include <cstring>

namespace guest {
uint32_t FlagIndexGetNth(uint32_t q0 = 0, uint32_t q1 = 0, uint32_t q2 = 0, ...);
uint32_t FlagIndexBitsUpTo(uint32_t q0 = 0, uint32_t q1 = 0, uint32_t q2 = 0, ...);
}  // namespace guest

int main() {
  std::memset(rtguest::kRam, 0, sizeof(rtguest::kRam));

  const uint32_t blk = 0x00400000;
  *(uint8_t*)rtguest::G(blk) = 0x0b;      // 3 bits
  *(uint8_t*)rtguest::G(blk + 1) = 0x01;  // 1 bit
  *(uint8_t*)rtguest::G(blk + 2) = 0xff;  // 8 bits

  assert(guest::FlagIndexGetNth(blk, 3, 0xffffffff) == 12);
  assert(guest::FlagIndexGetNth(blk, 2, 0xffffffff) == 4);   // byte count honored
  assert(guest::FlagIndexGetNth(blk, 0, 0xffffffff) == 0);   // empty range
  assert(guest::FlagIndexBitsUpTo(blk + 2, 1, 0xffffffff) == 8);
  return 0;
}
