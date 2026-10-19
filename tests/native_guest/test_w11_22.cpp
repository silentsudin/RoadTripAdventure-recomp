// Pins SetVehicleMode (0x002329a0): mode 3 clears the 0x6c-byte block, then
// writes the pseudocode's widths, sprite modes, and 0x80 values.
#include "guest/guest_shim.h"

#include <cassert>
#include <cstring>

namespace guest {
uint32_t SetVehicleMode(uint32_t, uint32_t, ...);
}

int main() {
  std::memset(rtguest::kRam, 0xff, RT_GUEST_RAM_SIZE);
  constexpr uint32_t state = 0x1000;
  *(uint32_t*)rtguest::G(state + 0x8004) = 0;
  *(uint32_t*)rtguest::G(state + 0x8008) = 0;

  guest::SetVehicleMode(state, 3);

  assert(*(uint32_t*)rtguest::G(state + 0x800c) == 3);
  assert(*(uint32_t*)rtguest::G(state + 0x8010) == 0x140);
  assert(*(uint32_t*)rtguest::G(state + 0x8014) == 0x40);
  assert(*(uint32_t*)rtguest::G(state + 0x8018) == 7);
  assert(*(uint32_t*)rtguest::G(state + 0x801c) == 6);
  assert(*(uint32_t*)rtguest::G(state + 0x8020) == 6);
  assert(*(uint32_t*)rtguest::G(state + 0x8024) == 6);
  assert(*(uint32_t*)rtguest::G(state + 0x8038) == 2);
  assert(*(uint32_t*)rtguest::G(state + 0x8058) == 0x80);
  assert(*(uint32_t*)rtguest::G(state + 0x8064) == 0x80);
}
