// guest:0x002835cc transliterated from Ghidra pseudocode — DO NOT hand-edit
// memchr
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

namespace {
// EE MMI 128-bit model for the packed ops below (Ghidra's undefined1[16]).
struct V16 { uint64_t lo, hi; };
inline V16 _pcpyh(V16 a) {  // broadcast low halfword to all 8 halfwords
  uint64_t h = a.lo & 0xffff;
  h |= h << 0x10;
  h |= h << 0x20;
  return {h, h};
}
inline V16 _pcpyld(V16 a, V16 b) { return {a.lo, b.lo}; }  // [rs.lo, rt.lo]
inline V16 _pcpyud(V16 a, V16 b) { return {a.hi, b.hi}; }  // [rs.hi, rt.hi]
inline V16 _pxor(V16 a, V16 b) { return {a.lo ^ b.lo, a.hi ^ b.hi}; }
inline V16 _psubb(V16 a, V16 b) {  // modular per-byte subtract (EE PSUBB)
  V16 r;
  for (int i = 0; i < 8; i++) {
    ((uint8_t*)&r.lo)[i] = (uint8_t)(((uint8_t*)&a.lo)[i] - ((uint8_t*)&b.lo)[i]);
    ((uint8_t*)&r.hi)[i] = (uint8_t)(((uint8_t*)&a.hi)[i] - ((uint8_t*)&b.hi)[i]);
  }
  return r;
}
inline V16 _pand(V16 a, V16 b) { return {a.lo & b.lo, a.hi & b.hi}; }
inline V16 _pnot(V16 a) { return {~a.lo, ~a.hi}; }  // Ghidra ~ on a 16-byte var
}  // namespace

uint32_t memchr(uint32_t __s, uint32_t __c, uint32_t __n, ...) {
  V16 auVar1;
  V16 auVar2;
  int iVar3;
  V16 auVar4;
  V16 auVar5;
  V16 auVar6;
  // dead register inputs: _pcpyld keeps only the .lo half of each source
  uint64_t in_v1_udw = 0;
  uint64_t in_t2_udw = 0;
  uint64_t in_a0_udw = 0;

  if ((0xf < __n) && ((__s & 0xf) == 0)) {
    auVar4.hi = in_t2_udw;
    auVar4.lo = (uint64_t)(__c & 0xff) * 0x101;
    auVar5.hi = in_v1_udw;
    auVar5.lo = 0x8080808080808080ULL;
    auVar4 = _pcpyh(auVar4);
    auVar4 = _pcpyld(auVar4,auVar4);
    auVar5 = _pcpyld(auVar5,auVar5);
    do {
      auVar1 = _pxor((*(V16*)rtguest::G(__s)),auVar4);
      auVar6.hi = in_a0_udw;
      auVar6.lo = 0x0101010101010101ULL;
      auVar2.hi = in_a0_udw;
      auVar2.lo = 0x0101010101010101ULL;
      auVar6 = _pcpyld(auVar6,auVar2);
      auVar6 = _psubb(auVar1,auVar6);
      auVar6 = _pand(auVar6,_pnot(auVar1));
      auVar6 = _pand(auVar6,auVar5);
      auVar2 = _pcpyud(auVar6,auVar4);
      if (auVar6.lo != 0 || auVar2.lo != 0) break;
      __n = (size_t)((int)__n + -0x10);
      __s = __s + 0x10;
    } while (0xf < __n);
  }
  iVar3 = (int)__n;
  while( true ) {
    iVar3 = iVar3 + -1;
    if (iVar3 == -1) {
      return 0;
    }
    // hardware compares an lbu (0x283688): zero-extended byte; Ghidra's (byte) cast is signed
    if ((uint64_t)(*(uint8_t*)rtguest::G(__s)) == (uint64_t)(__c & 0xff)) break;
    __s = __s + 1;
  }
  return __s;
}


RT_GUEST_FN(0x002835cc, guest::memchr)

}  // namespace guest
