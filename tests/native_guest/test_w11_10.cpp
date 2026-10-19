#include "../../src/native/guest/guest_shim.h"

#include <cassert>
#include <cstring>

namespace guest {
uint32_t lfVersionCheck(uint32_t, ...);
}

// Pins guest:0x0028c050 lfVersionCheck against values computed by hand from the
// pseudocode (build/decomp/fn/0028c050.c): it memcmps the loader version word at
// 0x177a028 against the word at 0x2e9138 and against the target of the pointer at
// 0x2e99a4, and returns true only when all three words are pairwise distinct.

static void setVersionWords(uint32_t a, uint32_t b, uint32_t c) {
  *static_cast<uint32_t*>(rtguest::G(0x177a028)) = a;
  *static_cast<uint32_t*>(rtguest::G(0x2e9138)) = b;
  *static_cast<uint32_t*>(rtguest::G(0x2e99a4)) = 0x2e9140;  // pointer target
  *static_cast<uint32_t*>(rtguest::G(0x2e9140)) = c;
}

int main() {
  std::memset(rtguest::kRam, 0, RT_GUEST_RAM_SIZE);

  // a == b: first memcmp == 0 -> false before the second compare
  setVersionWords(0x11223344, 0x11223344, 0x11223344);
  assert(guest::lfVersionCheck(0) == 0);

  // a == c: second memcmp == 0 -> false before the third compare
  setVersionWords(1, 2, 1);
  assert(guest::lfVersionCheck(0) == 0);

  // b == c: third memcmp == 0 -> bVar2 stays false
  setVersionWords(1, 2, 2);
  assert(guest::lfVersionCheck(0) == 0);

  // pairwise distinct: all three memcmps nonzero -> returns true
  setVersionWords(0x01020304, 0x05060708, 0x090a0b0c);
  assert(guest::lfVersionCheck(0) == 1);
}
