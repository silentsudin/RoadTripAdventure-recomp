// guest:0x0027d768 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __pack_d(uint32_t param_1, ...) {
  uint uVar1;
  ulong uVar2;
  ulong uVar3;
  ulong uVar4;
  
  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)param_1));
  uVar4 = 0;
  uVar2 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 4)));
  if (uVar1 < 2) {
    uVar4 = 0x7ff;
    uVar3 = uVar2 | 0x8000000000000;
    goto LAB_0027d824;
  }
  if (uVar1 != 4) {
    if (uVar1 == 2) {
      uVar3 = 0;
    }
    uVar3 = 0;
    if (uVar2 == 0) goto LAB_0027d824;
    uVar1 = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t)));
    if ((int)uVar1 < -0x3fe) {
      uVar3 = 0;
    }
    if ((int)uVar1 < 0x400) {
      uVar4 = (uint32_t)(int)(uVar1 + 0x3ff);
      if ((uVar2 & 0xff) == 0x80) {
        if ((uVar2 & 0x100) != 0) {
          uVar2 = uVar2 + 0x80;
        }
      }
      else {
        uVar2 = uVar2 + 0x7f;
      }
      if (uVar2 < 0x2000000000000000) {
        uVar3 = uVar2 >> 8;
      }
      else {
        uVar4 = (uint32_t)(int)(uVar1 + 0x400);
        uVar3 = uVar2 >> 9;
      }
    }
  }
  uVar4 = 0x7ff;
  uVar3 = 0;
LAB_0027d824:
  return uVar3 & 0xfffffffffffff | (uVar4 & 0x7ff) << 0x34 | (long)(int)(*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint32_t))) << 0x3f;
}


RT_GUEST_FN(0x0027d768, guest::__pack_d)

}  // namespace guest
