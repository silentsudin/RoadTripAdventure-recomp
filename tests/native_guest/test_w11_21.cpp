// Pins the translated byte/short table walks in W11-21 against their Ghidra branches.
#include "guest/guest_shim.h"

#include <cassert>
#include <cstring>

namespace guest {
uint32_t AdvanceOscillatingValue(uint32_t, ...);
}

int main() {
  std::memset(rtguest::kRam, 0, RT_GUEST_RAM_SIZE);
  constexpr uint32_t value = 0x1000;

  // 0x0023e018: forward range [0, 10) adds the byte step at 0x2a36c8.
  *(int16_t*)rtguest::G(0x2a36b0) = 0;
  *(int16_t*)rtguest::G(0x2a36b2) = 10;
  *(uint8_t*)rtguest::G(0x2a36c8) = 2;
  *(uint16_t*)rtguest::G(value) = 5;
  guest::AdvanceOscillatingValue(value);
  assert(*(uint16_t*)rtguest::G(value) == 7);

  // With the direction flag set, the reversed inclusive/exclusive comparison subtracts.
  *(int16_t*)rtguest::G(0x2a36b0) = 10;
  *(int16_t*)rtguest::G(0x2a36b2) = 0;
  *(uint8_t*)rtguest::G(0x335708) = 1;
  *(uint16_t*)rtguest::G(value) = 5;
  guest::AdvanceOscillatingValue(value);
  assert(*(uint16_t*)rtguest::G(value) == 3);
}
