// guest:0x0027f2f4 transliterated from Ghidra pseudocode — DO NOT hand-edit
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

uint32_t strlen(uint32_t __s, ...) {
  rtguest::FrameGuard rt_fg;
  uint32_t pauVar5;
  uint64_t uVar2;

  pauVar5 = __s;
  if ((__s & 7) == 0) {
    if ((__s & 0xf) == 0) {
      uint64_t x0 = *(uint64_t*)rtguest::G((uint32_t)__s);
      uint64_t x1 = *(uint64_t*)rtguest::G((uint32_t)(__s + 8));
      uint64_t m0 = zdet(x0), m1 = zdet(x1);
      if (m1 == 0 && m0 == 0) {
        do {
          pauVar5 = pauVar5 + 0x10;
          x0 = *(uint64_t*)rtguest::G((uint32_t)pauVar5);
          x1 = *(uint64_t*)rtguest::G((uint32_t)(pauVar5 + 8));
          m0 = zdet(x0);
          m1 = zdet(x1);
        } while (m1 == 0 && m0 == 0);
      }
    }
    else {
      uVar2 = *(uint64_t*)rtguest::G((uint32_t)__s) + 0xfefefefefefefeffULL &
              ~*(uint64_t*)rtguest::G((uint32_t)__s);
      while ((uVar2 & 0x8080808080808080ULL) == 0) {
        pauVar5 = pauVar5 + 8;
        uVar2 = *(uint64_t*)rtguest::G((uint32_t)pauVar5) + 0xfefefefefefefeffULL &
                ~*(uint64_t*)rtguest::G((uint32_t)pauVar5);
      }
    }
  }
  while (*(uint8_t*)rtguest::G((uint32_t)pauVar5) != 0) {
    pauVar5 = pauVar5 + 1;
  }
  return (uint32_t)((int)pauVar5 - (int)__s);
}


RT_GUEST_FN(0x0027f2f4, guest::strlen)

}  // namespace guest
