#include "../../src/native/guest/guest_shim.h"

#include <cassert>

namespace guest {
uint32_t _hi0bits(uint32_t, ...);
}

int main() {
  // 0x002839c0: hand-evaluated leading-zero counts from the mask cascade.
  assert(guest::_hi0bits(0) == 32);
  assert(guest::_hi0bits(0x80000000) == 0);
  assert(guest::_hi0bits(0x00f00000) == 8);
}
