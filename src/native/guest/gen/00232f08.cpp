// guest:0x00232f08 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetWheelSprite2(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  int iVar2;
  undefined4 uVar3;
uint32_t piVar4;
uint32_t puVar5;
  
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8008))) == 0x13) {
    iVar2 = 2;
    iVar1 = param_2 * 0x408 + param_1;
    piVar4 = (uint32_t)(iVar1 + 0x871c);
    if (param_3 == 0) {
      iVar2 = 0;
    }
    param_2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8004))) + param_2;
    if ((*(int32_t*)rtguest::G((uint32_t)piVar4)) != iVar2) {
      (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(piVar4)), 0, 0x2c));
      (*(int32_t*)rtguest::G((uint32_t)piVar4)) = iVar2;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8720))) = 0x140;
      if (param_2 == 0) {
        uVar3 = 0xa8;
      }
      else {
        uVar3 = 0x70;
        if (param_2 != 1) {
          uVar3 = 0x150;
        }
      }
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8724))) = uVar3;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8744))) = 0x40000000;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8728))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x872c))) = 2;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8730))) = 0x0029f948;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8734))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8740))) = 0x1e;
    }
  }
  else {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8008))) == 0x1a) {
      iVar1 = 2;
      if (param_3 == 0) {
        iVar1 = 0;
      }
      iVar2 = param_2 * 0x408 + param_1;
      piVar4 = (uint32_t)(iVar2 + 0x871c);
      param_2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8004))) + param_2;
      puVar5 = 0x0029f968;
      if ((*(int32_t*)rtguest::G((uint32_t)piVar4)) == iVar1) {
        return 0;
      }
      (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(piVar4)), 0, 0x2c));
      (*(int32_t*)rtguest::G((uint32_t)piVar4)) = iVar1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8720))) = 0x140;
      if (param_2 == 0) {
        iVar1 = 0xa8;
      }
      else {
        iVar1 = 0x70;
        if (param_2 != 1) {
          iVar1 = 0x150;
        }
      }
      iVar2 = 1;
    }
    else {
      iVar1 = 2;
      if (param_3 == 0) {
        iVar1 = 0;
      }
      iVar2 = param_2 * 0x408 + param_1;
      piVar4 = (uint32_t)(iVar2 + 0x871c);
      param_2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8004))) + param_2;
      puVar5 = 0x0029f988;
      if ((*(int32_t*)rtguest::G((uint32_t)piVar4)) == iVar1) {
        return 0;
      }
      (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(piVar4)), 0, 0x2c));
      (*(int32_t*)rtguest::G((uint32_t)piVar4)) = iVar1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8720))) = 0x140;
      if (param_2 == 0) {
        iVar1 = 0xa8;
      }
      else {
        iVar1 = 0x70;
        if (param_2 != 1) {
          iVar1 = 0x150;
        }
      }
      iVar2 = 0xd;
    }
    (*(int32_t*)rtguest::G((uint32_t)piVar4 + (uint32_t)(2) * sizeof(int32_t))) = iVar1;
    (*(int32_t*)rtguest::G((uint32_t)piVar4 + (uint32_t)(10) * sizeof(int32_t))) = 0x40000000;
    (*(int32_t*)rtguest::G((uint32_t)piVar4 + (uint32_t)(3) * sizeof(int32_t))) = 1;
    (*(int32_t*)rtguest::G((uint32_t)piVar4 + (uint32_t)(4) * sizeof(int32_t))) = iVar2;
    (*(int32_t*)rtguest::G((uint32_t)piVar4 + (uint32_t)(5) * sizeof(int32_t))) = (int)puVar5;
    (*(int32_t*)rtguest::G((uint32_t)piVar4 + (uint32_t)(6) * sizeof(int32_t))) = 1;
    (*(int32_t*)rtguest::G((uint32_t)piVar4 + (uint32_t)(9) * sizeof(int32_t))) = 0x1e;
  }
  return 0;
}


RT_GUEST_FN(0x00232f08, guest::SetWheelSprite2)

}  // namespace guest
