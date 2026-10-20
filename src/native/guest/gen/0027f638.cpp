// guest:0x0027f638 transliterated from Ghidra pseudocode — DO NOT hand-edit
// __sbprintf
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// pseudocode returns long; narrowed to the decl's uint32_t (value is a 32-bit int count / -1)
uint32_t __sbprintf(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  long lVar2;
  // FILE aFStack_480[7]: 0x480-byte frame; Ghidra field offsets as in 002826c0,
  // &__pad3 (0x400-byte buffer) at +0x60 (addiu $11,$sp,0x60 at 0x0027f64c)
  uint32_t aFStack_480 = rtguest::allocFrame(0x480);

  (*(int32_t*)rtguest::G(aFStack_480)) = (int)(aFStack_480 + 0x60);
  (*(uint16_t*)rtguest::G(aFStack_480 + 0xe)) = (*(uint16_t*)rtguest::G(param_1 + 0xe));
  (*(uint32_t*)rtguest::G(aFStack_480 + 0x54)) = (*(uint32_t*)rtguest::G(param_1 + 0x54));
  (*(uint16_t*)rtguest::G(aFStack_480 + 0xc)) = (*(uint16_t*)rtguest::G(param_1 + 0xc)) & 0xfffd;
  (*(uint32_t*)rtguest::G(aFStack_480 + 0x1c)) = (*(uint32_t*)rtguest::G(param_1 + 0x1c));
  (*(uint32_t*)rtguest::G(aFStack_480 + 0x24)) = (*(uint32_t*)rtguest::G(param_1 + 0x24));
  (*(uint32_t*)rtguest::G(aFStack_480 + 0x14)) = 0x400;
  (*(uint32_t*)rtguest::G(aFStack_480 + 8)) = 0x400;
  (*(uint32_t*)rtguest::G(aFStack_480 + 0x18)) = 0;
  (*(uint32_t*)rtguest::G(aFStack_480 + 0x10)) = (*(uint32_t*)rtguest::G(aFStack_480));
  lVar2 = (long)(int32_t)vfprintf(aFStack_480, param_2, param_3);  // arg-drop fix: a1/a2 live at jal 0x27f694 (disasm)
  if ((-1 < lVar2) && (iVar1 = fflush(aFStack_480), iVar1 != 0)) {
    lVar2 = -1;
  }
  if (((*(uint16_t*)rtguest::G(aFStack_480 + 0xc)) & 0x40) != 0) {
    (*(uint16_t*)rtguest::G(param_1 + 0xc)) = (*(uint16_t*)rtguest::G(param_1 + 0xc)) | 0x40;
  }
  return lVar2;
}


RT_GUEST_FN(0x0027f638, guest::__sbprintf)

}  // namespace guest
