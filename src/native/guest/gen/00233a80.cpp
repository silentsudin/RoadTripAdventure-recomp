// guest:0x00233a80 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateTimedPhaseTask(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x28, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x38, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x48, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x58) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x5c) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_3);  // walk: = param_3
    rtguest::setS32(3, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(6, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(8, (uint32_t)param_5);  // walk: = param_5
  }
  int iVar1;
  short sVar2;
  int iVar3;
  uint uVar4;
uint32_t puVar5;
  undefined4 uVar6;
  int iVar7;
  int iVar8;
uint32_t puVar9;
  
  iVar7 = (int)param_1;
  sVar2 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 2)));
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x1c)));
  if (sVar2 < 0x40) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0xfffeffff;
    RT_CALLRA(0x233aec, guest::uiFade((0x40 - sVar2) * 0x2000000));
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 2))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 2))) + 1;
  }
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0x4000) != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8))) = 0;
    return 0;
  }
  iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8)));
  if (iVar8 == 0x3c) {
    RT_CALLRA(0x233b24, guest::PlayUiSound(0x2d));
    iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8)));
  }
  else if (iVar8 == 0xf0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 4;
    iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8)));
  }
  iVar1 = iVar8 / 0x3c;
  if (iVar8 % 0x3c != 0) goto LAB_00233de0;
  if (param_3 == 1) {
    puVar9 = 0x0029f7e8;
    if (iVar1 != 0) {
      puVar9 = 0x0029f7c8 + iVar1 * 0x20;
    }
    uVar6 = 3;
    puVar5 = (uint32_t)(iVar3 + 0x80a4);
    if (iVar1 == 0) {
      uVar6 = 0;
    }
    iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8004)));
    (*(uint32_t*)rtguest::G((uint32_t)puVar5)) = uVar6;
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(puVar5)), 0, 0x2c));
    (*(uint32_t*)rtguest::G((uint32_t)puVar5)) = uVar6;
    uVar6 = 0x70;
    if (iVar8 != 0) {
      uVar6 = 0xe0;
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80ac))) = uVar6;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80a8))) = 0x140;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80b8))) = puVar9;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80bc))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80c8))) = 10;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80b0))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80b4))) = 1;
  }
  else if (param_3 < 2) {
    if (param_3 == 0) {
      puVar9 = 0x0029f7e8;
      if (iVar1 != 0) {
        puVar9 = 0x0029f7c8 + iVar1 * 0x20;
      }
      uVar6 = 3;
      puVar5 = (uint32_t)(iVar3 + 0x80a4);
      if (iVar1 == 0) {
        uVar6 = 0;
      }
      iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8004)));
      (*(uint32_t*)rtguest::G((uint32_t)puVar5)) = uVar6;
      (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(puVar5)), 0, 0x2c));
      (*(uint32_t*)rtguest::G((uint32_t)puVar5)) = uVar6;
      uVar6 = 0x70;
      if (iVar8 != 0) {
        uVar6 = 0xe0;
      }
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80ac))) = uVar6;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80a8))) = 0x140;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80b8))) = puVar9;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80bc))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80c8))) = 10;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80b0))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80b4))) = 1;
    }
    else {
LAB_00233d50:
      iVar8 = iVar8 / 0x3c;
      puVar9 = 0x0029f7e8;
      if (iVar8 != 0) {
        puVar9 = 0x0029f7c8 + iVar8 * 0x20;
      }
      uVar6 = 3;
      puVar5 = (uint32_t)(iVar3 + 0x80a4);
      if (iVar8 == 0) {
        uVar6 = 0;
      }
      (*(uint32_t*)rtguest::G((uint32_t)puVar5)) = uVar6;
      (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(puVar5)), 0, 0x2c));
      (*(uint32_t*)rtguest::G((uint32_t)puVar5)) = uVar6;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80a8))) = 0x140;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80ac))) = 0xe0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80b8))) = puVar9;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80bc))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80c8))) = 10;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80b0))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80b4))) = 1;
    }
  }
  else {
    if (param_3 != 2) goto LAB_00233d50;
    iVar8 = 0x29f868;
    if (iVar1 != 0) {
      iVar8 = iVar1 * 0x20 + 0x29f848;
    }
    uVar6 = 3;
    puVar5 = (uint32_t)(iVar3 + 0x80a4);
    if (iVar1 == 0) {
      uVar6 = 0;
    }
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8004)));
    (*(uint32_t*)rtguest::G((uint32_t)puVar5)) = uVar6;
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(puVar5)), 0, 0x2c));
    (*(uint32_t*)rtguest::G((uint32_t)puVar5)) = uVar6;
    uVar6 = 0x70;
    if (iVar1 != 0) {
      uVar6 = 0xe0;
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80a8))) = 0x140;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80ac))) = uVar6;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80b4))) = 0xd;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80b8))) = iVar8;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80bc))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80c8))) = 10;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80b0))) = 0;
  }
  iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8)));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80cc))) = 0x40000000;
LAB_00233de0:
  if (((iVar8 == 300) || ((*(int8_t*)rtguest::G(0x177adea)) != '\0')) || (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0x8000) != 0)) {
    if (param_5 == 0) {
      RT_CALLRA(0x233e0c, guest::PlayBgmSlot1());
    }
    if (param_4 == 0) {
      uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 0x104;
    }
    else {
      uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 4;
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = uVar4;
    puVar5 = (uint32_t)(iVar3 + 0x80a4);
    iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8004)));
    (*(uint32_t*)rtguest::G((uint32_t)puVar5)) = 0;
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(puVar5)), 0, 0x2c));
    (*(uint32_t*)rtguest::G((uint32_t)puVar5)) = 0;
    uVar6 = 0x70;
    if (iVar8 != 0) {
      uVar6 = 0xe0;
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80a8))) = 0x140;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80ac))) = uVar6;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80b8))) = 0x0029f7e8;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80b0))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80b4))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80bc))) = 1;
    iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x18)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80c8))) = 10;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80cc))) = 0x40000000;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 4))) = 0xffffffff;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x24))) = 1;
    RT_CALLRA(0x233eb4, guest::gs_tasks_kill(param_1));
  }
  return 0;
}


RT_GUEST_FN(0x00233a80, guest::UpdateTimedPhaseTask)

}  // namespace guest
