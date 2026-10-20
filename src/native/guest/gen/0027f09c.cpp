// guest:0x0027f09c transliterated from Ghidra pseudocode — DO NOT hand-edit
// strcmp (newlib r5900 SIMD: 16B psubw/zero-detect fast path, 8B word loop, byte tail).
// V16 MMI model per 0027ef70. in_a0/a2/a3_udw provably dead: only memory-derived
// halves reach the .lo comparisons.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

namespace {
struct V16 { uint64_t lo, hi; };
inline V16 _pcpyld(V16 a, V16 b) { return {a.lo, b.lo}; }
inline V16 _pcpyud(V16 a, V16 b) { return {a.hi, b.hi}; }
inline V16 operator~(V16 a) { return {~a.lo, ~a.hi}; }
inline V16 _pand(V16 a, V16 b) { return {a.lo & b.lo, a.hi & b.hi}; }
inline V16 _psubb(V16 a, V16 b) {  // 16 byte-lane subtracts (mod 256)
  rtguest::FrameGuard rt_fg(0);
  V16 r{0, 0};
  for (int i = 0; i < 8; ++i) {
    r.lo |= ((a.lo >> (i * 8)) - (b.lo >> (i * 8)) & 0xff) << (i * 8);
    r.hi |= ((a.hi >> (i * 8)) - (b.hi >> (i * 8)) & 0xff) << (i * 8);
  }
  return r;
}
inline V16 _psubw(V16 a, V16 b) {  // 8 word-lane subtracts (mod 65536)
  rtguest::FrameGuard rt_fg(0);
  V16 r{0, 0};
  for (int i = 0; i < 4; ++i) {
    r.lo |= ((a.lo >> (i * 16)) - (b.lo >> (i * 16)) & 0xffff) << (i * 16);
    r.hi |= ((a.hi >> (i * 16)) - (b.hi >> (i * 16)) & 0xffff) << (i * 16);
  }
  return r;
}
inline V16 ld16(uint32_t a) {
  rtguest::FrameGuard rt_fg(0);
  return {*(uint64_t*)rtguest::G(a), *(uint64_t*)rtguest::G(a + 8)};
}
}  // namespace

uint32_t strcmp(uint32_t __s1, uint32_t __s2, ...) {
  rtguest::FrameGuard rt_fg(0);
  uint8_t bVar1;
  int8_t cVar4;
  uint64_t uVar5;
  V16 in_a0_udw{0, 0};
  V16 in_a2_udw{0, 0};
  V16 in_a3_udw{0, 0};
  V16 auVar2, auVar3, auVar6, auVar7, auVar8, auVar9, auVar10;

  if ((((uint32_t)__s1 | (uint32_t)__s2) & 7) == 0) {
    auVar7 = {0x101010101010101ull, in_a3_udw.lo};
    auVar6 = {0x8080808080808080ull, in_a2_udw.lo};
    if ((((uint32_t)__s1 | (uint32_t)__s2) & 0xf) == 0) {
      auVar9 = _pcpyld(auVar7, auVar7);
      auVar10 = _pcpyld(auVar6, auVar6);
      auVar7 = _psubw(ld16(__s2), ld16(__s1));
      auVar8 = {(uint64_t)(int32_t)__s1, in_a0_udw.lo};
      auVar6 = _pcpyud(auVar7, auVar8);
      if (auVar6.lo == 0 && auVar7.lo == 0) {
        auVar6 = ld16(__s1);
        do {
          auVar7 = _psubb(auVar6, auVar9);
          auVar6 = _pand(auVar7, ~auVar6);
          auVar6 = _pand(auVar6, auVar10);
          auVar2 = {(uint64_t)(int32_t)__s1, in_a0_udw.lo};
          auVar7 = _pcpyud(auVar6, auVar2);
          __s1 = (uint32_t)__s1 + 0x10;
          if (auVar7.lo != 0 || auVar6.lo != 0) {
            return 0;
          }
          __s2 = (uint32_t)__s2 + 0x10;
          auVar6 = ld16(__s1);
          auVar8 = _psubw(auVar6, ld16(__s2));
          auVar3 = {(uint64_t)(int32_t)__s1, in_a0_udw.lo};
          auVar7 = _pcpyud(auVar8, auVar3);
        } while (auVar7.lo == 0 && auVar8.lo == 0);
        cVar4 = (int8_t)*(uint8_t*)rtguest::G((uint32_t)__s1);
      }
      else {
        cVar4 = (int8_t)*(uint8_t*)rtguest::G((uint32_t)__s1);
      }
    }
    else if (*(uint64_t*)rtguest::G((uint32_t)__s1) == *(uint64_t*)rtguest::G((uint32_t)__s2)) {
      uVar5 = *(uint64_t*)rtguest::G((uint32_t)__s1);
      do {
        __s1 = (uint32_t)__s1 + 8;
        if (((uVar5 + 0xfefefefefefefeffull) & ~uVar5 & 0x8080808080808080ull) != 0) {
          return 0;
        }
        __s2 = (uint32_t)__s2 + 8;
        uVar5 = *(uint64_t*)rtguest::G((uint32_t)__s1);
      } while (*(uint64_t*)rtguest::G((uint32_t)__s2) == uVar5);
      cVar4 = (int8_t)*(uint8_t*)rtguest::G((uint32_t)__s1);
    }
    else {
      cVar4 = (int8_t)*(uint8_t*)rtguest::G((uint32_t)__s1);
    }
  }
  else {
    cVar4 = (int8_t)*(uint8_t*)rtguest::G((uint32_t)__s1);
  }
  do {
    bVar1 = *(uint8_t*)rtguest::G((uint32_t)__s1);
    if (cVar4 == '\0') {
      return (uint32_t)bVar1 - (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)__s2));
    }
    if ((int32_t)cVar4 != (int32_t)(int8_t)*(uint8_t*)rtguest::G((uint32_t)__s2)) {
      bVar1 = *(uint8_t*)rtguest::G((uint32_t)__s1);
      return (uint32_t)bVar1 - (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)__s2));
    }
    __s1 = (uint32_t)__s1 + 1;
    __s2 = (uint32_t)__s2 + 1;
    cVar4 = (int8_t)*(uint8_t*)rtguest::G((uint32_t)__s1);
  } while (true);
}


RT_GUEST_FN(0x0027f09c, guest::strcmp)

}  // namespace guest
