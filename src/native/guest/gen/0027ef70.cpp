// guest:0x0027ef70 transliterated from Ghidra pseudocode — DO NOT hand-edit
// strcat (newlib r5900 SIMD strlen + tail strcpy). V16 MMI model per 0027e840.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

namespace {
// EE MMI 128-bit model (Ghidra's undefined1[16]).
struct V16 { uint64_t lo, hi; };
inline V16 _pcpyld(V16 a, V16 b) { return {a.lo, b.lo}; }  // [rs.lo, rt.lo]
inline V16 _pcpyud(V16 a, V16 b) { return {a.hi, b.hi}; }  // [rs.hi, rt.hi]
inline V16 operator~(V16 a) { return {~a.lo, ~a.hi}; }
inline V16 _pand(V16 a, V16 b) { return {a.lo & b.lo, a.hi & b.hi}; }
inline V16 _psubb(V16 a, V16 b) {  // 16 byte-lane subtracts (mod 256)
  rtguest::FrameGuard rt_fg;
  V16 r{0, 0};
  for (int i = 0; i < 8; ++i) {
    r.lo |= ((a.lo >> (i * 8)) - (b.lo >> (i * 8)) & 0xff) << (i * 8);
    r.hi |= ((a.hi >> (i * 8)) - (b.hi >> (i * 8)) & 0xff) << (i * 8);
  }
  return r;
}
inline V16 ld16(uint32_t a) {
  rtguest::FrameGuard rt_fg;
  return {*(uint64_t*)rtguest::G(a), *(uint64_t*)rtguest::G(a + 8)};
}
}  // namespace

uint32_t strcat(uint32_t __dest, uint32_t __src, ...) {
  rtguest::FrameGuard rt_fg;
  uint64_t uVar1;
  V16 in_v1_udw{0, 0};    // dead input: _pcpyld reads only lo halves
  V16 in_a0_udw{0, 0};
  V16 auVar2;
  V16 auVar3;
  uint32_t __dest_00;
  V16 auVar4;
  V16 auVar5;

  __dest_00 = __dest;
  if (((uint32_t)__dest & 7) == 0) {
    auVar2 = {0x101010101010101ull, in_v1_udw.lo};
    if (((uint32_t)__dest & 0xf) == 0) {
      auVar4 = _pcpyld(auVar2, auVar2);
      auVar3 = {0x8080808080808080ull, in_a0_udw.lo};
      auVar5 = {0x8080808080808080ull, in_a0_udw.lo};
      auVar5 = _pcpyld(auVar3, auVar5);
      auVar2 = _psubb(ld16(__dest), auVar4);
      auVar2 = _pand(auVar2, ~ld16(__dest));
      auVar3 = _pand(auVar2, auVar5);
      auVar2 = _pcpyud(auVar3, auVar3);
      if (auVar2.lo == 0 && auVar3.lo == 0) {
        do {
          __dest_00 = __dest_00 + 16;
          auVar2 = _psubb(ld16((uint32_t)__dest_00), auVar4);
          auVar2 = _pand(auVar2, ~ld16((uint32_t)__dest_00));
          auVar2 = _pand(auVar2, auVar5);
          auVar3 = _pcpyud(auVar2, auVar2);
        } while (auVar2.lo == 0 && auVar3.lo == 0);
      }
    }
    else {
      uVar1 = (*(uint64_t*)rtguest::G(__dest)) + 0xfefefefefefefeffull & ~(*(uint64_t*)rtguest::G(__dest));
      while ((uVar1 & 0x8080808080808080ull) == 0) {
        __dest_00 = __dest_00 + 8;  // (*arr + 8) is array-decay pointer arithmetic, not a load
        uVar1 = (*(uint64_t*)rtguest::G((uint32_t)__dest_00)) + 0xfefefefefefefeffull &
                ~(*(uint64_t*)rtguest::G((uint32_t)__dest_00));
      }
    }
  }
  for (; (*(uint8_t*)rtguest::G((uint32_t)__dest_00)) != 0; __dest_00 = __dest_00 + 1) {
  }
  ::strcpy((char*)rtguest::G((uint32_t)__dest_00), (char*)rtguest::G((uint32_t)__src));
  return __dest;
}


RT_GUEST_FN(0x0027ef70, guest::strcat)

}  // namespace guest
