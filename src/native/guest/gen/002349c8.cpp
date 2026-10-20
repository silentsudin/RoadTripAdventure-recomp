// guest:0x002349c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetRoadData4(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  undefined4 uVar2;
uint32_t __s;
  int iVar3;
  
  iVar3 = 0;
  iVar1 = param_2 * 0x408 + param_1;
  __s = (uint32_t)(iVar1 + 0x871c);
  if (param_3 != 0) {
    iVar3 = 2;
  }
  param_2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8004))) + param_2;
  if ((*(int32_t*)rtguest::G((uint32_t)__s)) != iVar3) {
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x2c));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8720))) = 0x140;
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
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x872c))) = 0xd;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8744))) = 0x40000000;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8724))) = uVar2;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8728))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8730))) = 0x0029f9a8;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8734))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8740))) = 0x1e;
  }
  return 0;
}


RT_GUEST_FN(0x002349c8, guest::SetRoadData4)

}  // namespace guest
