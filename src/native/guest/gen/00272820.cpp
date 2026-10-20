// guest:0x00272820 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// PS2 SIMD on lo/hi uint64 pairs (V16 convention, strcmp TU precedent).
struct V16 { uint64_t lo, hi; };
static inline V16 qextl(V16 a, V16 b) {  // _pextlw: interlace lower 32-bit words
  rtguest::FrameGuard rt_fg(0);
  return { (a.lo & 0xffffffff) | (b.lo << 32),
           (a.lo >> 32) | (b.hi & 0xffffffff00000000ULL) };
}
static inline V16 qextu(V16 a, V16 b) {  // _pextuw: interlace upper 32-bit words
  rtguest::FrameGuard rt_fg(0);
  return { (a.hi & 0xffffffff) | (b.hi << 32),
           (a.hi >> 32) | (b.hi >> 32 << 32) };
}
static inline V16 qpyld(V16 a, V16 b) { return { a.lo, b.lo }; }  // _pcpyld
static inline V16 qpyud(V16 a, V16 b) { return { a.hi, b.hi }; }  // _pcpyud

uint32_t mat4Transpose(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(0);
  auto row = [](uint32_t a) {
    return V16{ *(uint64_t*)rtguest::G(a), *(uint64_t*)rtguest::G(a + 8) };
  };
  V16 r0 = row(param_2), r1 = row(param_2 + 0x10);
  V16 r2 = row(param_2 + 0x20), r3 = row(param_2 + 0x30);

  V16 t2 = qextl(r1, r0);
  V16 t3 = qextu(r1, r0);
  V16 t4 = qextl(r3, r2);
  V16 t5 = qextu(r3, r2);
  V16 o1 = qpyld(t4, t2);
  V16 o2 = qpyud(t2, t4);
  V16 o4 = qpyld(t5, t3);
  V16 o3 = qpyud(t3, t5);

  const uint32_t* w;
  w = (const uint32_t*)&o1;  // little-endian lane order == memory order
  for (int i = 0; i < 4; i++) *(uint32_t*)rtguest::G((uint32_t)(param_1 + i * 4)) = w[i];
  w = (const uint32_t*)&o2;
  for (int i = 0; i < 4; i++) *(uint32_t*)rtguest::G((uint32_t)(param_1 + (4 + i) * 4)) = w[i];
  w = (const uint32_t*)&o4;
  for (int i = 0; i < 4; i++) *(uint32_t*)rtguest::G((uint32_t)(param_1 + (8 + i) * 4)) = w[i];
  w = (const uint32_t*)&o3;
  for (int i = 0; i < 4; i++) *(uint32_t*)rtguest::G((uint32_t)(param_1 + (0xc + i) * 4)) = w[i];
  return 0;
}


RT_GUEST_FN(0x00272820, guest::mat4Transpose)

}  // namespace guest
