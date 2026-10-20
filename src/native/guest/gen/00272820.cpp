// guest:0x00272820 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// MIPS 0x272820..0x272860 (MMI decoded): lq r0..r3; PEXTLW $12=(r0.x,r1.x,r0.y,r1.y), PEXTUW $13=(r0.z,r1.z,r0.w,r1.w),
// PEXTLW $14=(r2.x,r3.x,r2.y,r3.y), PEXTUW $15=(r2.z,r3.z,r2.w,r3.w); PCPYLD/PCPYUD give the 4 columns; sq to out.
// i.e. a plain 4x4 transpose (fn/00272820.c). The earlier V16 helpers interleaved the wrong words.
uint32_t mat4Transpose(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(0);
  uint32_t m[16];
  for (int i = 0; i < 16; i++) m[i] = *(uint32_t*)rtguest::G(param_2 + 4u * i);  // all lq before any sq (in-place safe)
  for (int r = 0; r < 4; r++)
    for (int c = 0; c < 4; c++) *(uint32_t*)rtguest::G(param_1 + 4u * (r * 4 + c)) = m[c * 4 + r];
  return 0;
}


RT_GUEST_FN(0x00272820, guest::mat4Transpose)

}  // namespace guest
