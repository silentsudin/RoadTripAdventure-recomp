// Pins W11-26 audit: ResetCarEffectSlots (0x00225588) zeroes the contiguous
// effect-slot block [0x0172ff70, 0x01730bf0) in three memsets (0x800, 0x80,
// 0x400 — back-to-back tables) and must leave the guard bytes either side.
#include "guest/guest_shim.h"

#include <cassert>
#include <cstring>

namespace guest {
uint32_t ResetCarEffectSlots(uint32_t q0 = 0, ...);
}

int main() {
  std::memset(rtguest::kRam, 0xFF, RT_GUEST_RAM_SIZE);
  *(uint8_t*)rtguest::G(0x0172ff6f) = 0x5A;
  *(uint8_t*)rtguest::G(0x01730bf0) = 0xA5;
  guest::ResetCarEffectSlots();
  for (uint32_t a = 0x0172ff70; a < 0x01730bf0; ++a) assert(*(uint8_t*)rtguest::G(a) == 0);
  assert(*(uint8_t*)rtguest::G(0x0172ff6f) == 0x5A);
  assert(*(uint8_t*)rtguest::G(0x01730bf0) == 0xA5);
}
