// guest:0x0027e5f8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// SIMD (_psubb/_pand zero-detect) byte-exact: 0x80 bit per zero byte.
static inline uint64_t zdet(uint64_t x) {
  rtguest::FrameGuard rt_fg;
  uint64_t m = 0;
  for (int s = 0; s < 64; s += 8)
    if (((x >> s) & 0xff) == 0) m |= 0x80ULL << s;
  return m;
}

uint32_t memcmp(uint32_t __s1, uint32_t __s2, uint32_t __n, ...) {
  rtguest::FrameGuard rt_fg;
  uint32_t puVar1;
  uint32_t iVar4;
  uint32_t uVar5;

  if ((__n > 0xf) && (((__s1 | __s2) & 0xf) == 0)) {
    do {
      uVar5 = (uint32_t)(__n < 0x20);  // CONCAT71(0, __n < 0x20); only ==0 tested (pcpyud drops it otherwise)
      uint64_t x = (*(uint64_t*)rtguest::G((uint32_t)__s2)) ^ (*(uint64_t*)rtguest::G((uint32_t)__s1));
      uint64_t y = (*(uint64_t*)rtguest::G((uint32_t)(__s1 + 8))) ^ (*(uint64_t*)rtguest::G((uint32_t)(__s2 + 8)));
      if ((x | y) != 0) break;
      __n = __n + -0x10;
      __s1 = __s1 + 0x10;
      __s2 = __s2 + 0x10;
    } while (uVar5 == 0);
  }
  iVar4 = (int)__n;
  while (true) {
    iVar4 = iVar4 + -1;
    if (iVar4 == -1) {
      return 0;
    }
    puVar1 = __s1;
    __s1 = __s1 + 1;
    if ((uint32_t)(uint8_t)*(uint8_t*)rtguest::G((uint32_t)puVar1) !=
        (uint32_t)(uint8_t)*(uint8_t*)rtguest::G((uint32_t)__s2)) break;
    __s2 = __s2 + 1;
  }
  return (uint32_t)(uint8_t)*(uint8_t*)rtguest::G((uint32_t)puVar1) -
         (uint32_t)(uint8_t)*(uint8_t*)rtguest::G((uint32_t)__s2);
}


RT_GUEST_FN(0x0027e5f8, guest::memcmp)

}  // namespace guest
