#include "../../src/native/guest/guest_shim.h"

#include <cassert>
#include <cstring>

namespace guest {
uint32_t _lo0bits(uint32_t, ...);
}

int main() {
  std::memset(rtguest::kRam, 0, RT_GUEST_RAM_SIZE);
  constexpr uint32_t word = 0x1000;

  // 0x00283a40: 0x28 has three trailing zero bits, leaving 5.
  *static_cast<uint32_t*>(rtguest::G(word)) = 0x28;
  assert(guest::_lo0bits(word) == 3);
  assert(*static_cast<uint32_t*>(rtguest::G(word)) == 5);

  // 0x00283a40: zero takes the explicit all-zero return path.
  *static_cast<uint32_t*>(rtguest::G(word)) = 0;
  assert(guest::_lo0bits(word) == 32);
}
