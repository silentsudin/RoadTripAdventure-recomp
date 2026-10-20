// guest:0x0027f430 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// pseudocode's first 16-byte check uses _psubb(~x,ones) & ~x & highs:
// flags every byte <= 0x7e (superset of zero bytes -> conservative early exit only).
static inline uint64_t zdet_inv(uint64_t x) {
  rtguest::FrameGuard rt_fg;
  uint64_t m = 0;
  for (int s = 0; s < 64; s += 8) {
    uint64_t c = (~x >> s) & 0xff;
    if (((c - 1) & 0xff) & c & 0x80) m |= 0x80ULL << s;
  }
  return m;
}

// standard _psubb(x,ones) & ~x & highs: 0x80 bit per zero byte.
static inline uint64_t zdet(uint64_t x) {
  rtguest::FrameGuard rt_fg;
  uint64_t m = 0;
  for (int s = 0; s < 64; s += 8)
    if (((x >> s) & 0xff) == 0) m |= 0x80ULL << s;
  return m;
}

uint32_t strncpy(uint32_t __dest, uint32_t __src, uint32_t __n, ...) {
  rtguest::FrameGuard rt_fg;
  bool bVar1;
  uint8_t cVar2;
  uint32_t uVar7;
  uint32_t uVar8;
  uint32_t uVar9;
  uint32_t uVar10;
  uint32_t uVar11;
  uint64_t uVar14;
  uint32_t uVar17;
  int iVar15;
  uint32_t pcVar16;

  uVar7 = __src | __dest;
  pcVar16 = __dest;
  if ((uVar7 & 7) != 0) goto LAB_0027f598;
  uVar17 = 8;
  if ((uVar7 & 0xf) == 0) {
    uVar17 = 0x10;
  }
  if ((uVar7 & 0xf) == 0) {
    if (__n < uVar17) goto LAB_0027f598;
    uint64_t x0 = *(uint64_t*)rtguest::G((uint32_t)__src);
    uint64_t x1 = *(uint64_t*)rtguest::G((uint32_t)(__src + 8));
    if (zdet_inv(x0) != 0 || zdet_inv(x1) != 0) goto LAB_0027f598;
    uVar8 = *(uint32_t*)rtguest::G((uint32_t)__src);
    uVar9 = *(uint32_t*)rtguest::G((uint32_t)(__src + 4));
    uVar10 = *(uint32_t*)rtguest::G((uint32_t)(__src + 8));
    uVar11 = *(uint32_t*)rtguest::G((uint32_t)(__src + 0xc));
    __n = (uint32_t)((int)__n + -0x10);
    __src = __src + 0x10;
    *(uint32_t*)rtguest::G((uint32_t)__dest) = uVar8;
    *(uint32_t*)rtguest::G((uint32_t)(__dest + 4)) = uVar9;
    *(uint32_t*)rtguest::G((uint32_t)(__dest + 8)) = uVar10;
    *(uint32_t*)rtguest::G((uint32_t)(__dest + 0xc)) = uVar11;
    pcVar16 = __dest + 0x10;
    if (__n < 0x10) goto LAB_0027f598;
    x0 = *(uint64_t*)rtguest::G((uint32_t)__src);
    x1 = *(uint64_t*)rtguest::G((uint32_t)(__src + 8));
    if (zdet(x0) != 0 || zdet(x1) != 0) goto LAB_0027f598;
    uVar14 = *(uint64_t*)rtguest::G((uint32_t)__src);
  }
  else {
    if (__n < uVar17) goto LAB_0027f598;
    if ((*(uint64_t*)rtguest::G((uint32_t)__src) + 0xfefefefefefefeffULL &
         ~*(uint64_t*)rtguest::G((uint32_t)__src) & 0x8080808080808080ULL) != 0)
      goto LAB_0027f598;
    uVar14 = *(uint64_t*)rtguest::G((uint32_t)__src);
  }
  while (true) {
    __n = (uint32_t)((int)__n + -8);
    __src = __src + 8;
    *(uint64_t*)rtguest::G((uint32_t)pcVar16) = uVar14;
    pcVar16 = pcVar16 + 8;
    uint64_t q = *(uint64_t*)rtguest::G((uint32_t)__src);
    if ((__n < 8) || ((q - 0x0101010101010101ULL & ~q) & 0x8080808080808080ULL) != 0) break;
    uVar14 = q;
  }
LAB_0027f598:
  do {
    if (__n == 0) {
      return __dest;
    }
    cVar2 = *(uint8_t*)rtguest::G((uint32_t)__src);
    iVar15 = (int)__n;
    __n = (uint32_t)(iVar15 + -1);
    __src = __src + 1;
    *(uint8_t*)rtguest::G((uint32_t)pcVar16) = cVar2;
    pcVar16 = pcVar16 + 1;
  } while (cVar2 != 0);
  iVar15 = iVar15 + -2;
  if (__n != 0) {
    do {
      *(uint8_t*)rtguest::G((uint32_t)pcVar16) = 0;
      pcVar16 = pcVar16 + 1;
      bVar1 = iVar15 != 0;
      iVar15 = iVar15 + -1;
    } while (bVar1);
  }
  return __dest;
}


RT_GUEST_FN(0x0027f430, guest::strncpy)

}  // namespace guest
