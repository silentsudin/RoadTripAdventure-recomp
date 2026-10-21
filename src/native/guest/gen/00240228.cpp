// guest:0x00240228 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateCameraTransition(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
  }
  byte bVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  
  iVar4 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 8))) == 0) {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x27))) = 5;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x20))) = 0xb4;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c))) = 0;
    (*(uint32_t*)rtguest::G(0x2a2bb0)) = rtguest::f2bits(900.0f);
    (*(uint32_t*)rtguest::G(0x2a2bb4)) = rtguest::f2bits(25.0f);
    (*(uint32_t*)rtguest::G(0x2a2bb8)) = rtguest::f2bits(300.0f);
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x14))) - 0x255a8U < 0x1fa4) {
      (*(uint8_t*)rtguest::G(0x335738)) = 1;
    }
    uVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x14))) - 0xe484;
    if (0x190c7 < uVar3) {
      (*(uint8_t*)rtguest::G(0x335738)) = 4;
    }
    if (uVar3 < 0x17124) {
      (*(uint8_t*)rtguest::G(0x335738)) = 6;
      (*(uint32_t*)rtguest::G(0x2a2bb4)) = rtguest::f2bits(100.0f);
    }
  }
  switch((*(uint8_t*)rtguest::G(0x335738))) {
  case 0:
    (*(float*)rtguest::G(0x2a2bb4)) = (*(float*)rtguest::G(0x2a2bb4)) - (*(float*)rtguest::G(0x332e84));
    if ((*(float*)rtguest::G(0x2a2bb4)) <= 25.0) {
      (*(uint8_t*)rtguest::G(0x335738)) = 1;
      EmptyStub();
      return 0;
    }
    break;
  case 1:
    if (((*(float*)rtguest::G(0x332e88)) < (*(float*)rtguest::G(0x2a2bb8))) &&
       ((*(float*)rtguest::G(0x2a2bb8)) = (*(float*)rtguest::G(0x2a2bb8)) - (*(float*)rtguest::G(0x332e8c)), (*(float*)rtguest::G(0x2a2bb8)) <= (*(float*)rtguest::G(0x332e88)))) {
      (*(uint32_t*)rtguest::G(0x2a2bb8)) = (*(uint32_t*)rtguest::G(0x332e88));
      (*(uint8_t*)rtguest::G(0x335738)) = 2;
    }
    break;
  case 2:
    bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x27)));
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
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x27))) = bVar1;
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
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x27))) = bVar1;
      }
LAB_00240454:
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x20)));
    }
    (*(float*)rtguest::G(0x2a2bb8)) = (*(float*)rtguest::G(0x2a2bb8)) + rtguest::fdiv(((float)(iVar2 + -0xb4) * (*(float*)rtguest::G(0x332e90))), 720.0);
    (*(float*)rtguest::G(0x2a2bb0)) = (*(float*)rtguest::G(0x2a2bb0)) + rtguest::fdiv(((float)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c))) + -0xb4) * (*(float*)rtguest::G(0x332e90))), 720.0);
    if (0x2770d < (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x14)))) {
      (*(uint8_t*)rtguest::G(0x335738)) = 3;
      EmptyStub();
      return 0;
    }
    break;
  case 3:
    if (((*(float*)rtguest::G(0x2a2bb0)) < 900.0) &&
       ((*(float*)rtguest::G(0x2a2bb0)) = (*(float*)rtguest::G(0x2a2bb0)) + (*(float*)rtguest::G(0x332e94)), 900.0 <= (*(float*)rtguest::G(0x2a2bb0)))) {
      (*(uint32_t*)rtguest::G(0x2a2bb0)) = rtguest::f2bits(900.0f);
    }
    if ((900.0 < (*(float*)rtguest::G(0x2a2bb0))) &&
       ((*(float*)rtguest::G(0x2a2bb0)) = (*(float*)rtguest::G(0x2a2bb0)) - (*(float*)rtguest::G(0x332e98)), (*(float*)rtguest::G(0x2a2bb0)) <= 900.0)) {
      (*(uint32_t*)rtguest::G(0x2a2bb0)) = rtguest::f2bits(900.0f);
    }
    if (((*(float*)rtguest::G(0x2a2bb8)) < 300.0) &&
       ((*(float*)rtguest::G(0x2a2bb8)) = (*(float*)rtguest::G(0x2a2bb8)) + (*(float*)rtguest::G(0x332e9c)), 300.0 <= (*(float*)rtguest::G(0x2a2bb8)))) {
      (*(uint32_t*)rtguest::G(0x2a2bb8)) = rtguest::f2bits(300.0f);
    }
    if ((300.0 < (*(float*)rtguest::G(0x2a2bb8))) &&
       ((*(float*)rtguest::G(0x2a2bb8)) = (*(float*)rtguest::G(0x2a2bb8)) - (*(float*)rtguest::G(0x332ea0)), (*(float*)rtguest::G(0x2a2bb8)) <= 300.0)) {
      (*(uint32_t*)rtguest::G(0x2a2bb8)) = rtguest::f2bits(300.0f);
    }
    if (((*(float*)rtguest::G(0x2a2bb0)) == 900.0) && ((*(float*)rtguest::G(0x2a2bb8)) == 300.0)) {
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
    (*(float*)rtguest::G(0x2a2bb4)) = (*(float*)rtguest::G(0x2a2bb4)) + (*(float*)rtguest::G(0x332ea4));
    if (100.0 <= (*(float*)rtguest::G(0x2a2bb4))) {
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
    rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2ec8e0u)));
    gs_tasks_kill(param_1);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x00240228, guest::UpdateCameraTransition)

}  // namespace guest
