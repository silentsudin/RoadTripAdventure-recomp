// guest:0x00240228 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateCameraTransition(uint32_t param_1, uint32_t param_2, ...) {
  byte bVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  
  iVar4 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 8))) == 0) {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x27))) = 5;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x20))) = 0xb4;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c))) = 0;
    (*(uint32_t*)rtguest::G(0x2a2bb0)) = 900.0;
    (*(uint32_t*)rtguest::G(0x2a2bb4)) = 25.0;
    (*(uint32_t*)rtguest::G(0x2a2bb8)) = 300.0;
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x14))) - 0x255a8U < 0x1fa4) {
      (*(uint8_t*)rtguest::G(0x335738)) = 1;
    }
    uVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x14))) - 0xe484;
    if (0x190c7 < uVar3) {
      (*(uint8_t*)rtguest::G(0x335738)) = 4;
    }
    if (uVar3 < 0x17124) {
      (*(uint8_t*)rtguest::G(0x335738)) = 6;
      (*(uint32_t*)rtguest::G(0x2a2bb4)) = 100.0;
    }
  }
  switch((*(uint8_t*)rtguest::G(0x335738))) {
  case 0:
    (*(uint32_t*)rtguest::G(0x2a2bb4)) = (*(uint32_t*)rtguest::G(0x2a2bb4)) - (*(uint32_t*)rtguest::G(0x332e84));
    if ((*(uint32_t*)rtguest::G(0x2a2bb4)) <= 25.0) {
      (*(uint8_t*)rtguest::G(0x335738)) = 1;
      EmptyStub();
      return 0;
    }
    break;
  case 1:
    if (((*(uint32_t*)rtguest::G(0x332e88)) < (*(uint32_t*)rtguest::G(0x2a2bb8))) &&
       ((*(uint32_t*)rtguest::G(0x2a2bb8)) = (*(uint32_t*)rtguest::G(0x2a2bb8)) - (*(uint32_t*)rtguest::G(0x332e8c)), (*(uint32_t*)rtguest::G(0x2a2bb8)) <= (*(uint32_t*)rtguest::G(0x332e88)))) {
      (*(uint32_t*)rtguest::G(0x2a2bb8)) = (*(uint32_t*)rtguest::G(0x332e88));
      (*(uint8_t*)rtguest::G(0x335738)) = 2;
    }
    break;
  case 2:
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x27)));
    if ((bVar1 & 1) == 0) {
      if (((bVar1 & 2) != 0) &&
         (iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c))) + -1, (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c))) = iVar2, iVar2 < 1)) {
        bVar1 = bVar1 & 0xfd | 1;
        goto LAB_002403f8;
      }
    }
    else {
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c))) + 1;
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c))) = iVar2;
      if (0x167 < iVar2) {
        bVar1 = bVar1 & 0xfe | 2;
LAB_002403f8:
        (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x27))) = bVar1;
      }
    }
    if ((bVar1 & 4) == 0) {
      if ((bVar1 & 8) != 0) {
        iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x20))) + -1;
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x20))) = iVar2;
        if (iVar2 < 1) {
          bVar1 = bVar1 & 0xf7 | 4;
          goto LAB_00240450;
        }
        goto LAB_00240454;
      }
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x20)));
    }
    else {
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x20))) + 1;
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x20))) = iVar2;
      if (0x167 < iVar2) {
        bVar1 = bVar1 & 0xfb | 8;
LAB_00240450:
        (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x27))) = bVar1;
      }
LAB_00240454:
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x20)));
    }
    (*(uint32_t*)rtguest::G(0x2a2bb8)) = (*(uint32_t*)rtguest::G(0x2a2bb8)) + ((float)(iVar2 + -0xb4) * (*(uint32_t*)rtguest::G(0x332e90))) / 720.0;
    (*(uint32_t*)rtguest::G(0x2a2bb0)) = (*(uint32_t*)rtguest::G(0x2a2bb0)) + ((float)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c))) + -0xb4) * (*(uint32_t*)rtguest::G(0x332e90))) / 720.0;
    if (0x2770d < (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x14)))) {
      (*(uint8_t*)rtguest::G(0x335738)) = 3;
      EmptyStub();
      return 0;
    }
    break;
  case 3:
    if (((*(uint32_t*)rtguest::G(0x2a2bb0)) < 900.0) &&
       ((*(uint32_t*)rtguest::G(0x2a2bb0)) = (*(uint32_t*)rtguest::G(0x2a2bb0)) + (*(uint32_t*)rtguest::G(0x332e94)), 900.0 <= (*(uint32_t*)rtguest::G(0x2a2bb0)))) {
      (*(uint32_t*)rtguest::G(0x2a2bb0)) = 900.0;
    }
    if ((900.0 < (*(uint32_t*)rtguest::G(0x2a2bb0))) &&
       ((*(uint32_t*)rtguest::G(0x2a2bb0)) = (*(uint32_t*)rtguest::G(0x2a2bb0)) - (*(uint32_t*)rtguest::G(0x332e98)), (*(uint32_t*)rtguest::G(0x2a2bb0)) <= 900.0)) {
      (*(uint32_t*)rtguest::G(0x2a2bb0)) = 900.0;
    }
    if (((*(uint32_t*)rtguest::G(0x2a2bb8)) < 300.0) &&
       ((*(uint32_t*)rtguest::G(0x2a2bb8)) = (*(uint32_t*)rtguest::G(0x2a2bb8)) + (*(uint32_t*)rtguest::G(0x332e9c)), 300.0 <= (*(uint32_t*)rtguest::G(0x2a2bb8)))) {
      (*(uint32_t*)rtguest::G(0x2a2bb8)) = 300.0;
    }
    if ((300.0 < (*(uint32_t*)rtguest::G(0x2a2bb8))) &&
       ((*(uint32_t*)rtguest::G(0x2a2bb8)) = (*(uint32_t*)rtguest::G(0x2a2bb8)) - (*(uint32_t*)rtguest::G(0x332ea0)), (*(uint32_t*)rtguest::G(0x2a2bb8)) <= 300.0)) {
      (*(uint32_t*)rtguest::G(0x2a2bb8)) = 300.0;
    }
    if (((*(uint32_t*)rtguest::G(0x2a2bb0)) == 900.0) && ((*(uint32_t*)rtguest::G(0x2a2bb8)) == 300.0)) {
      (*(uint8_t*)rtguest::G(0x335738)) = 4;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x20))) = 0xb4;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x27))) = 5;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c))) = 0;
      EmptyStub();
      return 0;
    }
    break;
  case 4:
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x14))) - 0xe484U < 0x17124) {
      (*(uint8_t*)rtguest::G(0x335738)) = 5;
    }
    break;
  case 5:
    (*(uint32_t*)rtguest::G(0x2a2bb4)) = (*(uint32_t*)rtguest::G(0x2a2bb4)) + (*(uint32_t*)rtguest::G(0x332ea4));
    if (100.0 <= (*(uint32_t*)rtguest::G(0x2a2bb4))) {
      (*(uint8_t*)rtguest::G(0x335738)) = 6;
      EmptyStub();
      return 0;
    }
    break;
  case 6:
    if (0x255a7 < (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x14)))) {
      (*(uint8_t*)rtguest::G(0x335738)) = 0;
    }
    break;
  case 7:
    ::printf((char *)rtguest::G((uint32_t)(0x2ec8e0u)));
    gs_tasks_kill(param_1);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x00240228, guest::UpdateCameraTransition)

}  // namespace guest
