// guest:0x0027f1e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// SIMD (_psubb/_pand zero-detect) byte-exact: 0x80 bit per zero byte.
static inline uint64_t zdet(uint64_t x) {
  rtguest::FrameGuard rt_fg(0);
  uint64_t m = 0;
  for (int s = 0; s < 64; s += 8)
    if (((x >> s) & 0xff) == 0) m |= 0x80ULL << s;
  return m;
}

uint32_t strcpy(uint32_t __dest, uint32_t __src, ...) {
  rtguest::FrameGuard rt_fg(0);
  uint8_t cVar1;
  uint32_t puVar6;
  uint32_t uVar8;
  uint32_t uVar9;
  uint32_t uVar10;
  uint32_t uVar11;
  uint64_t uVar12;
  uint64_t uVar2;

  puVar6 = __dest;
  if ((((__src | __dest)) & 7) == 0) {
    if ((((__src | __dest)) & 0xf) == 0) {
      uVar8 = *(uint32_t*)rtguest::G((uint32_t)__src);
      uVar9 = *(uint32_t*)rtguest::G((uint32_t)(__src + 4));
      uVar10 = *(uint32_t*)rtguest::G((uint32_t)(__src + 8));
      uVar11 = *(uint32_t*)rtguest::G((uint32_t)(__src + 0xc));
      uint64_t x0 = *(uint64_t*)rtguest::G((uint32_t)__src);
      uint64_t x1 = *(uint64_t*)rtguest::G((uint32_t)(__src + 8));
      uint64_t m0 = zdet(x0), m1 = zdet(x1);
      if (m0 == 0 && m1 == 0) {
        do {
          *(uint32_t*)rtguest::G((uint32_t)puVar6) = uVar8;
          *(uint32_t*)rtguest::G((uint32_t)(puVar6 + 4)) = uVar9;
          *(uint32_t*)rtguest::G((uint32_t)(puVar6 + 8)) = uVar10;   // pseudocode (puVar6+1) as ulong*: +8 bytes
          *(uint32_t*)rtguest::G((uint32_t)(puVar6 + 0xc)) = uVar11;
          __src = __src + 0x10;
          uVar8 = *(uint32_t*)rtguest::G((uint32_t)__src);
          uVar9 = *(uint32_t*)rtguest::G((uint32_t)(__src + 4));
          uVar10 = *(uint32_t*)rtguest::G((uint32_t)(__src + 8));
          uVar11 = *(uint32_t*)rtguest::G((uint32_t)(__src + 0xc));
          x0 = *(uint64_t*)rtguest::G((uint32_t)__src);
          x1 = *(uint64_t*)rtguest::G((uint32_t)(__src + 8));
          m0 = zdet(x0);
          m1 = zdet(x1);
          puVar6 = puVar6 + 0x10;  // pseudocode puVar6 + 2 as ulong*: 16 bytes
        } while (m0 == 0 && m1 == 0);
      }
    }
    else {
      uVar12 = *(uint64_t*)rtguest::G((uint32_t)__src);
      uVar2 = uVar12 + 0xfefefefefefefeffULL & ~uVar12;
      while ((uVar2 & 0x8080808080808080ULL) == 0) {
        *(uint64_t*)rtguest::G((uint32_t)puVar6) = uVar12;
        __src = __src + 8;
        uVar12 = *(uint64_t*)rtguest::G((uint32_t)__src);
        puVar6 = puVar6 + 8;
        uVar2 = uVar12 + 0xfefefefefefefeffULL & ~uVar12;
      }
    }
  }
  do {
    cVar1 = *(uint8_t*)rtguest::G((uint32_t)__src);
    __src = __src + 1;
    *(uint8_t*)rtguest::G((uint32_t)puVar6) = cVar1;
    puVar6 = puVar6 + 1;
  } while (cVar1 != 0);
  return __dest;
}


RT_GUEST_FN(0x0027f1e0, guest::strcpy)

}  // namespace guest
