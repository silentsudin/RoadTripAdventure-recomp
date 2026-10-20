// guest:0x0027da50 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __pack_f(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(0);
  int iVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  
  uVar4 = 0;
  uVar2 = (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(int32_t)));
  if ((*(int32_t*)rtguest::G((uint32_t)param_1)) != 2) {
    uVar3 = 0;
    if (uVar2 == 0) goto LAB_0027dad4;
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int32_t)));
    if (iVar1 < -0x7e) {
      uVar3 = 0;
      goto LAB_0027dad4;
    }
    if (iVar1 < 0x81) {
      uVar4 = iVar1 + 0x7f;
      if ((uVar2 & 0x7f) == 0x40) {
        if ((uVar2 & 0x80) != 0) {
          uVar2 = uVar2 + 0x40;
        }
      }
      else {
        uVar2 = uVar2 + 0x3f;
      }
      if ((int)uVar2 < 0) {
        uVar4 = iVar1 + 0x80;
        uVar3 = uVar2 >> 8;
      }
      else {
        uVar3 = uVar2 >> 7;
      }
      goto LAB_0027dad4;
    }
    uVar4 = 0xff;
  }
  uVar3 = 0;
LAB_0027dad4:
  return uVar3 & 0x7fffff | (uVar4 & 0xff) << 0x17 | (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t))) << 0x1f;
}


RT_GUEST_FN(0x0027da50, guest::__pack_f)

}  // namespace guest
