// guest:0x00268e40 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t shopPartsSellMenuTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = (uint32_t)(int32_t)param_2;
  }
  char cVar1;
  byte bVar2;
  undefined4 uVar3;
  undefined1 uVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  undefined8 uVar8;
  long lVar9;
uint32_t puVar10;
  int iVar11;
  uint uVar12;
  int iVar13;
  uint uVar14;
  
  iVar11 = (int)param_1;
  uVar12 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x1c))) & 1;
  uVar14 = (int)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x1c))) >> 1 & 1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 8))) == 0) {
    uVar8 = uiMenuCreate(param_1);
    LayoutTextItems(uVar8,0x2dc870,0xe,6);
    CenterRectangle(uVar8,5);
    iVar13 = (int)uVar8;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x5f))) = 1;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0xe))) = 1;
    if (uVar14 == 0) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0xc))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0xc))) | (uint32_t)uVar12;
    }
    else {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0xc))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0xc))) | 2;
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 2))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 2))) + 0x18;
    }
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x27))) = 0;
    return 0;
  }
  bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x25)));
  iVar13 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28)));
  if (bVar2 == 1) {
    if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x24))) != '\0') && (iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 4))), iVar6 != -1)) {
      iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002dc838 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0xd))) * 4)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0xe))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0xe))) | 0x80;
      DestroyTaskRecord(param_1,param_2);
      uVar4 = CountFlaggedSlots(iVar7,iVar6,uVar12,0xf);
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x27))) = uVar4;
      if (uVar14 == 0) {
        lVar9 = DispatchByIndex15(iVar7,iVar6);
        if (lVar9 == 0) {
          uVar14 = uVar12 | iVar7 << 0x10 | iVar6 << 0x18;
        }
        else {
          lVar9 = CountFlaggedSlots(iVar7,iVar6,uVar12,0xf);
          if (lVar9 != -1) {
            gs_tasks_spawn(param_1,0x268890,uVar12 | iVar7 << 0x10 | iVar6 << 0x18);
            (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x20))) = 1;
            (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x25))) = 3;
            goto LAB_00269460;
          }
          uVar14 = uVar12 | iVar6 << 0x18 | iVar7 << 0x10 | 0x100;
        }
      }
      else {
        lVar9 = DispatchByIndex15(iVar7,iVar6);
        if (lVar9 == 0) {
          uVar14 = uVar12 | iVar6 << 0x18 | iVar7 << 0x10 | 0x400;
        }
        else {
          lVar9 = CountFlaggedSlots(iVar7,iVar6,uVar12,0xf);
          uVar14 = uVar12 ^ 1;
          if (lVar9 == -1) {
            uVar14 = uVar12 | iVar6 << 0x18 | iVar7 << 0x10 | 0x500;
          }
          else {
            iVar5 = ComputeScaledPartValue(iVar7,iVar6,uVar14,0);
            if ((*(uint32_t*)rtguest::G((uint32_t)0x177fdb4 + (uint32_t)(uVar14 * 0xd12) * 4)) < iVar5) {
              uVar14 = uVar12 | iVar6 << 0x18 | iVar7 << 0x10 | 0x600;
            }
            else {
              lVar9 = TestFlaggedSlotBit(iVar7,iVar6,uVar14);
              if (lVar9 == 0) {
                uVar14 = uVar12 | iVar6 << 0x18 | iVar7 << 0x10;
                gs_tasks_spawn(param_1,0x268890,uVar14 | 0x200);
                gs_tasks_spawn(param_1,0x268890,uVar14 | 0x300);
                (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x20))) = 2;
                (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x25))) = 3;
                goto LAB_00269460;
              }
              uVar14 = uVar12 | iVar6 << 0x18 | iVar7 << 0x10 | 0x700;
            }
          }
        }
      }
      gs_tasks_spawn(param_1,0x268428,uVar14);
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x20))) = 1;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x25))) = 2;
    }
  }
  else if (bVar2 < 2) {
    if (bVar2 == 0) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0xe))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0xe))) & 0x7f;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x25))) = 1;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x26))) = 1;
    }
  }
  else if (bVar2 == 2) {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x24))) != '\0') {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x25))) = 0;
    }
  }
  else if (bVar2 == 3) {
    uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002dc838 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0xd))) * 4)));
    uVar8 = FindFlaggedSlotIndex(uVar3,uVar12,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x27))),0xf);
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x20))) < 0) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x25))) = 0;
    }
    else if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x20))) == 0) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x25))) = 0;
      if (uVar14 == 0) {
        ClearFlaggedSlotBit(uVar3,uVar8,uVar12);
        iVar6 = DispatchByIndex15(uVar3,uVar8);
        SpendTimerBudget(-iVar6 / 2,uVar12);
        cVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x27)));
      }
      else {
        uVar14 = uVar12 ^ 1;
        ClearFlaggedSlotBit(uVar3,uVar8,uVar12);
        iVar6 = ComputeScaledPartValue(uVar3,uVar8,uVar12,1);
        SpendTimerBudget(-iVar6,uVar12);
        SetFlaggedSlotBit(uVar3,uVar8,uVar14);
        uVar8 = ComputeScaledPartValue(uVar3,uVar8,uVar14,0);
        SpendTimerBudget(uVar8,uVar14);
        cVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x27)));
      }
      if ((cVar1 != '\0') && (lVar9 = FindFlaggedSlotIndex(uVar3,uVar12,cVar1,0xf), lVar9 == -1)) {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x27))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x27))) + -1;
      }
    }
    else if (uVar14 == 0) {
      iVar6 = DispatchByIndex15(uVar3,uVar8);
      DrawScoreAndRemaining(-iVar6 / 2,param_2);
    }
    else {
      if (uVar12 == 0) {
        iVar6 = ComputeScaledPartValue(uVar3,uVar8,0,1);
        iVar6 = -iVar6;
        iVar7 = ComputeScaledPartValue(uVar3,uVar8,1,0);
      }
      else {
        iVar6 = ComputeScaledPartValue(uVar3,uVar8,0,0);
        iVar7 = ComputeScaledPartValue(uVar3,uVar8,1,1);
        iVar7 = -iVar7;
      }
      DrawSlotScoreNumber(0x28,0xac,0);
      puVar10 = (uint32_t)((*(uint32_t*)rtguest::G(0x177fdb4)) - iVar6);
      if (999999 < (*(uint32_t*)rtguest::G(0x177fdb4)) - iVar6) {
        puVar10 = 0x000f423f;
      }
      DrawDialogTextLine(0x28,0x182,puVar10,0x177fd9f);
      uiDrawIcon20(0x74,((uint32_t)((*(uint64_t*)rtguest::G((uint32_t)param_2)) >> 1) & 7) + 0x171,7);
      DrawSlotScoreNumber(0x1b8,0xac,1);
      puVar10 = (uint32_t)((*(uint32_t*)rtguest::G(0x17831fc)) - iVar7);
      if (999999 < (*(uint32_t*)rtguest::G(0x17831fc)) - iVar7) {
        puVar10 = 0x000f423f;
      }
      DrawDialogTextLine(0x1b8,0x182,puVar10,0x17831e7);
      uiDrawIcon20(0x204,((uint32_t)((*(uint64_t*)rtguest::G((uint32_t)param_2)) >> 1) & 7) + 0x171,7);
    }
  }
LAB_00269460:
  cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0xd)));
  lVar9 = uiMenuWindow(iVar13,param_2,0x2dc870);
  if (lVar9 != -1) {
    if (cVar1 != (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0xd)))) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x27))) = 0;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x26))) = 1;
    }
    if (((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0xe))) == '\0') && ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x26))) != '\0')) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x26))) = 0;
      DestroyTaskRecord(param_1);
      gs_tasks_spawn(param_1,0x246250,
                     (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002dc838 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0xd))) * 4))) |
                     (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x27))) << 8 | uVar12 << 0x14);
    }
    return 0;
  }
  iVar11 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x18)));
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x24))) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 4))) = 0xffffffff;
  gs_tasks_kill(param_1);
  return 0;
}


RT_GUEST_FN(0x00268e40, guest::shopPartsSellMenuTask)

}  // namespace guest
