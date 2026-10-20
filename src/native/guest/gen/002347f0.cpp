// guest:0x002347f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetRoadData2(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
  int iVar1;
  undefined4 uVar2;
uint32_t __s;
  int iVar3;
  
  iVar3 = 0;
  iVar1 = param_2 * 0x408 + param_1;
  __s = (uint32_t)(iVar1 + 0x86f0);
  if (param_3 != 0) {
    iVar3 = 3;
  }
  param_2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8004))) + param_2;
  if ((*(int32_t*)rtguest::G((uint32_t)__s)) != iVar3) {
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x2c));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x86f4))) = 0x140;
    (*(int32_t*)rtguest::G((uint32_t)__s)) = iVar3;
    if (param_2 == 0) {
      uVar2 = 0xa8;
    }
    else {
      uVar2 = 0x70;
      if (param_2 != 1) {
        uVar2 = 0x150;
      }
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8700))) = 0xc;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8718))) = 0x40000000;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x86f8))) = uVar2;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x86fc))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8704))) = 0x0029f928;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8708))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8714))) = 0x1e;
  }
  return 0;
}


RT_GUEST_FN(0x002347f0, guest::SetRoadData2)

}  // namespace guest
