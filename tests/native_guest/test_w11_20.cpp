// Pins bit helpers against the Ghidra loops, including high-bit byte input.
#include "guest/guest_shim.h"

#include <cassert>
#include <cstring>

namespace guest {
uint32_t CountSetBits(uint32_t, ...);
uint32_t FindNthSetBit(uint32_t, uint32_t, ...);
uint32_t CountSetBitsInByteArray(uint32_t, uint32_t, ...);
uint32_t CountDecimalDigits(uint32_t, ...);
}

int main() {
  std::memset(rtguest::kRam, 0, RT_GUEST_RAM_SIZE);
  assert(guest::CountSetBits(0xf0f00101u) == 10);
  assert(guest::FindNthSetBit(0x91u, 0) == 0);
  assert(guest::FindNthSetBit(0x91u, 2) == 7);
  assert(guest::FindNthSetBit(0x91u, 3) == 0xffffffffu);

  constexpr uint32_t bytes = 0x1000;
  rtguest::kRam[bytes] = 0x80;
  rtguest::kRam[bytes + 1] = 0xff;
  rtguest::kRam[bytes + 2] = 3;
  assert(guest::CountSetBitsInByteArray(bytes, 3) == 11);
  assert(guest::CountDecimalDigits(static_cast<uint32_t>(-1)) == 1);
}
