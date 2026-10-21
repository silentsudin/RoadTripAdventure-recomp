// guest:0x00268e40 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t shopPartsSellMenuTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x28, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x38, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x48, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x58) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x5c) = 0x00000000u;
    *(uint32_t*)rtguest::G(rt_sv+0x0) = (uint32_t)(int32_t)param_2;
    rtguest::setS32(3, (uint32_t)param_1);  // walk: = param_1
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
    uVar8 = RT_CALLRA(0x268e8c, guest::uiMenuCreate(param_1));
    RT_CALLRA(0x268ea8, guest::LayoutTextItems(uVar8,0x2dc870,0xe,6));
    RT_CALLRA(0x268eb4, guest::CenterRectangle(uVar8,5));
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
      RT_CALLRA(0x268f98, guest::DestroyTaskRecord(param_1,param_2));
      uVar4 = RT_CALLRA(0x268fac, guest::CountFlaggedSlots(iVar7,iVar6,uVar12,0xf));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x27))) = uVar4;
      if (uVar14 == 0) {
        lVar9 = RT_CALLRA(0x268fc0, guest::DispatchByIndex15(iVar7,iVar6));
        if (lVar9 == 0) {
          uVar14 = uVar12 | iVar7 << 0x10 | iVar6 << 0x18;
        }
        else {
          lVar9 = RT_CALLRA(0x268ff8, guest::CountFlaggedSlots(iVar7,iVar6,uVar12,0xf));
          if (lVar9 != -1) {
            RT_CALLRA(0x26903c, guest::gs_tasks_spawn(param_1,0x268890,uVar12 | iVar7 << 0x10 | iVar6 << 0x18));
            (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x20))) = 1;
            (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x25))) = 3;
            goto LAB_00269460;
          }
          uVar14 = uVar12 | iVar6 << 0x18 | iVar7 << 0x10 | 0x100;
        }
      }
      else {
        lVar9 = RT_CALLRA(0x26905c, guest::DispatchByIndex15(iVar7,iVar6));
        if (lVar9 == 0) {
          uVar14 = uVar12 | iVar6 << 0x18 | iVar7 << 0x10 | 0x400;
        }
        else {
          lVar9 = RT_CALLRA(0x269098, guest::CountFlaggedSlots(iVar7,iVar6,uVar12,0xf));
          uVar14 = uVar12 ^ 1;
          if (lVar9 == -1) {
            uVar14 = uVar12 | iVar6 << 0x18 | iVar7 << 0x10 | 0x500;
          }
          else {
            iVar5 = RT_CALLRA(0x2690d4, guest::ComputeScaledPartValue(iVar7,iVar6,uVar14,0));
            if ((*(uint32_t*)rtguest::G((uint32_t)0x177fdb4 + (uint32_t)(uVar14 * 0xd12) * 4)) < iVar5) {
              uVar14 = uVar12 | iVar6 << 0x18 | iVar7 << 0x10 | 0x600;
            }
            else {
              lVar9 = RT_CALLRA(0x269124, guest::TestFlaggedSlotBit(iVar7,iVar6,uVar14));
              if (lVar9 == 0) {
                uVar14 = uVar12 | iVar6 << 0x18 | iVar7 << 0x10;
                RT_CALLRA(0x26914c, guest::gs_tasks_spawn(param_1,0x268890,uVar14 | 0x200));
                RT_CALLRA(0x269188, guest::gs_tasks_spawn(param_1,0x268890,uVar14 | 0x300));
                (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x20))) = 2;
                (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x25))) = 3;
                goto LAB_00269460;
              }
              uVar14 = uVar12 | iVar6 << 0x18 | iVar7 << 0x10 | 0x700;
            }
          }
        }
      }
      RT_CALLRA(0x269198, guest::gs_tasks_spawn(param_1,0x268428,uVar14));
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
        RT_CALLRA(0x269220, guest::ClearFlaggedSlotBit(uVar3,uVar8,uVar12));
        iVar6 = RT_CALLRA(0x26922c, guest::DispatchByIndex15(uVar3,uVar8));
        RT_CALLRA(0x269244, guest::SpendTimerBudget(-iVar6 / 2,uVar12));
        cVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x27)));
      }
      else {
        uVar14 = uVar12 ^ 1;
        RT_CALLRA(0x269264, guest::ClearFlaggedSlotBit(uVar3,uVar8,uVar12));
        iVar6 = RT_CALLRA(0x269278, guest::ComputeScaledPartValue(uVar3,uVar8,uVar12,1));
        RT_CALLRA(0x269284, guest::SpendTimerBudget(-iVar6,uVar12));
        RT_CALLRA(0x269294, guest::SetFlaggedSlotBit(uVar3,uVar8,uVar14));
        uVar8 = RT_CALLRA(0x2692a8, guest::ComputeScaledPartValue(uVar3,uVar8,uVar14,0));
        RT_CALLRA(0x2692b4, guest::SpendTimerBudget(uVar8,uVar14));
        cVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x27)));
      }
      if ((cVar1 != '\0') && (lVar9 = FindFlaggedSlotIndex(uVar3,uVar12,cVar1,0xf), lVar9 == -1)) {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x27))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x27))) + -1;
      }
    }
    else if (uVar14 == 0) {
      iVar6 = RT_CALLRA(0x269300, guest::DispatchByIndex15(uVar3,uVar8));
      RT_CALLRA(0x269318, guest::DrawScoreAndRemaining(-iVar6 / 2,param_2));
    }
    else {
      if (uVar12 == 0) {
        iVar6 = RT_CALLRA(0x269338, guest::ComputeScaledPartValue(uVar3,uVar8,0,1));
        iVar6 = -iVar6;
        iVar7 = RT_CALLRA(0x269350, guest::ComputeScaledPartValue(uVar3,uVar8,1,0));
      }
      else {
        iVar6 = RT_CALLRA(0x269368, guest::ComputeScaledPartValue(uVar3,uVar8,0,0));
        iVar7 = RT_CALLRA(0x269380, guest::ComputeScaledPartValue(uVar3,uVar8,1,1));
        iVar7 = -iVar7;
      }
      RT_CALLRA(0x2693a0, guest::DrawSlotScoreNumber(0x28,0xac,0));
      puVar10 = (uint32_t)((*(uint32_t*)rtguest::G(0x177fdb4)) - iVar6);
      if (999999 < (*(uint32_t*)rtguest::G(0x177fdb4)) - iVar6) {
        puVar10 = 0x000f423f;
      }
      RT_CALLRA(0x2693c8, guest::DrawDialogTextLine(0x28,0x182,puVar10,0x177fd9f));
      RT_CALLRA(0x2693f4, guest::uiDrawIcon20(0x74,((uint32_t)((*(uint64_t*)rtguest::G((uint32_t)param_2)) >> 1) & 7) + 0x171,7));
      RT_CALLRA(0x269404, guest::DrawSlotScoreNumber(0x1b8,0xac,1));
      puVar10 = (uint32_t)((*(uint32_t*)rtguest::G(0x17831fc)) - iVar7);
      if (999999 < (*(uint32_t*)rtguest::G(0x17831fc)) - iVar7) {
        puVar10 = 0x000f423f;
      }
      RT_CALLRA(0x269424, guest::DrawDialogTextLine(0x1b8,0x182,puVar10,0x17831e7));
      RT_CALLRA(0x269450, guest::uiDrawIcon20(0x204,((uint32_t)((*(uint64_t*)rtguest::G((uint32_t)param_2)) >> 1) & 7) + 0x171,7));
    }
  }
LAB_00269460:
  cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0xd)));
  lVar9 = (int32_t)RT_CALLRA(0x269468, guest::uiMenuWindow(iVar13,param_2,0x2dc870));
  if (lVar9 != -1) {
    if (cVar1 != (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0xd)))) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x27))) = 0;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x26))) = 1;
    }
    if (((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0xe))) == '\0') && ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x26))) != '\0')) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x26))) = 0;
      RT_CALLRA(0x2694b0, guest::DestroyTaskRecord(param_1));
      RT_CALLRA(0x2694e8, guest::gs_tasks_spawn(param_1,0x246250,
                     (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002dc838 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0xd))) * 4))) |
                     (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x27))) << 8 | uVar12 << 0x14));
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
