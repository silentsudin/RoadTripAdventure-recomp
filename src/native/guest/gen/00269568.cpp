// guest:0x00269568 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t shopPartsBuyMenuTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(64);
  char cVar1;
  byte bVar2;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  undefined1 uVar6;
  int iVar7;
  undefined8 uVar8;
  undefined8 uVar9;
  long lVar10;
  uint uVar11;
  int iVar12;
  int iVar13;
  uint uVar14;
  
  iVar12 = (int)param_1;
  uVar14 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x1c))) & 1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 8))) == 0) {
    uVar8 = uiMenuCreate(param_1);
    LayoutTextItems(uVar8,0x2dc8e0,0xe,6);
    CenterRectangle(uVar8,5);
    iVar13 = (int)uVar8;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x5f))) = 1;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0xe))) = 1;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0xc))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0xc))) | (uint32_t)uVar14;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x27))) = 0;
    return 0;
  }
  bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x25)));
  iVar13 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x28)));
  if (bVar2 == 1) {
    if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x24))) != '\0') && (iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 4))), iVar3 != -1)) {
      iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002dc8a8 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0xd))) * 4)));
      iVar7 = DispatchByIndex15(iVar4,iVar3);
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0xe))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0xe))) | 0x80;
      DestroyTaskRecord(param_1);
      uVar6 = countFlaggedSlotsAlt((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x23))),iVar4,iVar3);
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x27))) = uVar6;
      if ((int)(*(uint32_t*)rtguest::G((uint32_t)0x177fdb4 + (uint32_t)(uVar14 * 0xd12) * 4)) < iVar7) {
        uVar11 = uVar14 | iVar3 << 0x18 | iVar4 << 0x10 | 0x200;
      }
      else {
        lVar10 = TestFlaggedSlotBit(iVar4,iVar3,uVar14);
        if (lVar10 == 0) {
          gs_tasks_spawn(param_1,0x268890,uVar14 | iVar3 << 0x18 | iVar4 << 0x10 | 0x100);
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x25))) = 3;
          goto LAB_00269804;
        }
        uVar11 = uVar14 | iVar3 << 0x18 | iVar4 << 0x10 | 0x300;
      }
      gs_tasks_spawn(param_1,0x268428,uVar11);
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x25))) = 2;
    }
  }
  else if (bVar2 < 2) {
    if (bVar2 == 0) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0xe))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0xe))) & 0x7f;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x25))) = 1;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x26))) = 1;
    }
  }
  else if (bVar2 == 2) {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x24))) != '\0') {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x25))) = 0;
    }
  }
  else if (bVar2 == 3) {
    uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002dc8a8 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0xd))) * 4)));
    uVar8 = findFlaggedSlotIndexAlt
                      ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x23))),uVar5,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x27))));
    uVar9 = DispatchByIndex15(uVar5,uVar8);
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x24))) != '\0') {
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 4))) == 0) {
        SetFlaggedSlotBit(uVar5,uVar8,uVar14);
        SpendTimerBudget(uVar9,uVar14);
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x25))) = 0;
      }
      else {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x25))) = 0;
      }
    }
    DrawScoreAndRemaining(uVar9,param_2);
  }
LAB_00269804:
  cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0xd)));
  lVar10 = uiMenuWindow(iVar13,param_2,0x2dc8e0);
  if (lVar10 != -1) {
    if (cVar1 != (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0xd)))) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x27))) = 0;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x26))) = 1;
    }
    if (((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0xe))) == '\0') && ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x26))) != '\0')) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x26))) = 0;
      DestroyTaskRecord(param_1);
      gs_tasks_spawn(param_1,0x246250,
                     (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002dc8a8 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0xd))) * 4))) |
                     (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x27))) << 8 | uVar14 << 0x14);
    }
    return 0;
  }
  iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x18)));
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x24))) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 4))) = 0xffffffff;
  gs_tasks_kill(param_1);
  return 0;
}


RT_GUEST_FN(0x00269568, guest::shopPartsBuyMenuTask)

}  // namespace guest
