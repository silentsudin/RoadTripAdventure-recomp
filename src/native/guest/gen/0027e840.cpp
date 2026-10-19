// guest:0x0027e840 transliterated from Ghidra pseudocode — DO NOT hand-edit
// memset
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

namespace {
// EE MMI 128-bit model for the _pcpyh/_pcpyld ops below (Ghidra's undefined1[16]).
struct V16 { uint64_t lo, hi; };
inline V16 _pcpyh(V16 a) {  // broadcast low halfword to all 8 halfwords
  uint64_t h = a.lo & 0xffff;
  h |= h << 0x10;
  h |= h << 0x20;
  return {h, h};
}
inline V16 _pcpyld(V16 a, V16 b) { return {a.lo, b.lo}; }  // [rs.lo, rt.lo]
}  // namespace

uint32_t memset(uint32_t __s, uint32_t __c, uint32_t __n, ...) {
  bool bVar1;
  uint32_t puVar2;
  V16 auVar3;
  int iVar4;
  uint64_t in_t0_udw = 0;  // dead input: _pcpyh reads only the low halfword
  V16 auVar5;

  puVar2 = __s;
  if ((7 < __n) && ((__s & 0xf) == 0)) {
    auVar3.lo = ((uint64_t)(__c & 0xff) << 8) | (uint64_t)(__c & 0xff);
    auVar3.hi = in_t0_udw;
    auVar3 = _pcpyh(auVar3);
    bVar1 = __n < 8;
    if (0x1f < __n) {
      auVar5 = _pcpyld(auVar3,auVar3);
      do {
        (*(int32_t*)rtguest::G(puVar2)) = (int32_t)(uint32_t)auVar5.lo;
        (*(int32_t*)rtguest::G(puVar2 + 4)) = (int32_t)(uint32_t)(auVar5.lo >> 0x20);
        (*(int32_t*)rtguest::G(puVar2 + 8)) = (int32_t)(uint32_t)auVar5.hi;
        (*(int32_t*)rtguest::G(puVar2 + 0xc)) = (int32_t)(uint32_t)(auVar5.hi >> 0x20);
        __n = (size_t)((int)__n + -0x20);
        (*(int32_t*)rtguest::G(puVar2 + 0x10)) = (int32_t)(uint32_t)auVar5.lo;
        (*(int32_t*)rtguest::G(puVar2 + 0x14)) = (int32_t)(uint32_t)(auVar5.lo >> 0x20);
        (*(int32_t*)rtguest::G(puVar2 + 0x18)) = (int32_t)(uint32_t)auVar5.hi;
        (*(int32_t*)rtguest::G(puVar2 + 0x1c)) = (int32_t)(uint32_t)(auVar5.hi >> 0x20);
        puVar2 = puVar2 + 0x20;
      } while (0x1f < __n);
      bVar1 = __n < 8;
    }
    while (!bVar1) {
      (*(uint64_t*)rtguest::G(puVar2)) = auVar3.lo;
      __n = (size_t)((int)__n + -8);
      puVar2 = puVar2 + 8;
      bVar1 = __n < 8;
    }
  }
  iVar4 = (int)__n;
  while (iVar4 = iVar4 + -1, iVar4 != -1) {
    (*(int8_t*)rtguest::G(puVar2)) = (int8_t)__c;
    puVar2 = puVar2 + 1;
  }
  return __s;
}


RT_GUEST_FN(0x0027e840, guest::memset)

}  // namespace guest
