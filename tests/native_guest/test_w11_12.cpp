// Pins 0x00203f40 against its Ghidra loop: ordinary glyph, two-byte control,
// and a high-bit Japanese lead byte each take their distinct width path.
#include "guest/guest_shim.h"

#include <cassert>
#include <cstring>

namespace guest {
uint32_t uiTextWidth(uint32_t, ...);
}

int main() {
  std::memset(rtguest::kRam, 0, RT_GUEST_RAM_SIZE);
  constexpr uint32_t text = 0x1000;
  // 0x00203f40, input "A^B\\x80Z": 5 + 7 + 20 = 32; the lead byte skips Z.
  rtguest::kRam[0x29a860 + 'A'] = 5;
  rtguest::kRam[0x29a860 + 'B'] = 7;
  rtguest::kRam[0x29a860 + 'Z'] = 3;
  const uint8_t input[] = {'A', '^', 'B', 0x80, 'Z', 0};
  std::memcpy(rtguest::G(text), input, sizeof(input));
  assert(guest::uiTextWidth(text) == 32);
}
