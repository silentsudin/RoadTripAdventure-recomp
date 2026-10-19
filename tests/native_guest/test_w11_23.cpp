// Pins 0x0024e7d8 against the Ghidra byte/escape loop: printable characters
// expand to two-byte glyph codes, high-bit input copies as two bytes, and the
// count-limited path NUL-terminates the destination.
#include "guest/guest_shim.h"

#include <cassert>
#include <cstring>

namespace guest {
uint32_t CopyDialogueTextBounded(uint32_t, uint32_t, uint32_t, ...);
}

int main() {
  std::memset(rtguest::kRam, 0, RT_GUEST_RAM_SIZE);
  constexpr uint32_t src = 0x1000, dst = 0x1100;
  std::memcpy(rtguest::G(src), "A1!", 4);
  std::memcpy(rtguest::G(0x2a9b3a), "Aa", 2);
  std::memcpy(rtguest::G(0x2a9b38), "Bb", 2);
  std::memcpy(rtguest::G(0x2a9b40), "Cc", 2);  // '!' = 0x21 -> 0x2a9afe + 0x42
  assert(guest::CopyDialogueTextBounded(dst, src, 0) == dst);
  assert(std::memcmp(rtguest::G(dst), "AaBcCc", 6) == 0);
  rtguest::kRam[dst + 6] = 0xff;
  std::memcpy(rtguest::G(src), "XY", 3);
  assert(guest::CopyDialogueTextBounded(dst, src, 1) == dst);
  assert(rtguest::kRam[dst + 2] == 0);  // iVar5 becomes negative before copying Y.
}
