// guest:0x00236858 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateRankingsAndAdvance(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(128);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x68, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0x70, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x38, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x48, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x58, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x78) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x7c) = 0x00000000u;
    rtguest::setS32(6, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(7, (uint32_t)param_2);  // walk: = param_2
  }
uint32_t asStack_80 = rtguest::allocFrame(32);
    uint32_t uStack_60 = rtguest::allocFrame(4);
uint32_t pcVar1;
uint32_t pcVar2;
  char cVar3;
  ushort uVar4;
  undefined8 uVar5;
  undefined8 uVar6;
  uint uVar7;
uint32_t psVar8;
  undefined4 uVar9;
uint32_t pcVar10;
uint32_t psVar11;
  int iVar12;
  int iVar13;
  short sVar14;
  int iVar15;
  undefined8 uVar16;
  int iVar17;
uint32_t psVar18;
  
  RT_CALLRA(0x2368a0, guest::uiDrawPanelCells(2,4,0x1c,0xe,0));
  RT_CALLRA(0x2368b8, guest::uiDrawTextCell(3,5,0x333848,0));
  RT_CALLRA(0x2368d0, guest::uiDrawTextCell(10,5,0x2ec4c0,0));
  RT_CALLRA(0x2368e8, guest::uiDrawTextCell(0x15,5,0x2ec4d0,0));
  psVar8 = asStack_80 + 1;
  iVar15 = 0;
  iVar13 = 0;
  do {
    sVar14 = 0;
    (*(int16_t*)rtguest::G((uint32_t)psVar8 + (uint32_t)(iVar13 * 2) * sizeof(int16_t))) = (short)iVar13;
    if (-1 < (*(int32_t*)rtguest::G(0x1782a70))) {
      pcVar10 = (uint32_t)(iVar15 + iVar13 + 0x1782a76);
      iVar12 = (*(int32_t*)rtguest::G(0x1782a70)) + 1;
      sVar14 = 0;
      do {
        pcVar1 = pcVar10 + -2;
        iVar12 = iVar12 + -1;
        pcVar2 = pcVar10 + -1;
        cVar3 = (*(int8_t*)rtguest::G((uint32_t)pcVar10));
        pcVar10 = pcVar10 + 0x18;
        sVar14 = sVar14 + (*(int8_t*)rtguest::G((uint32_t)pcVar1)) + (short)(*(int8_t*)rtguest::G((uint32_t)pcVar2)) + (short)cVar3;
      } while (iVar12 != 0);
    }
    iVar12 = iVar13 + 1;
    (*(int16_t*)rtguest::G((uint32_t)asStack_80 + (uint32_t)(iVar13 * 2) * 2)) = sVar14;
    iVar15 = iVar15 + 2;
    iVar13 = iVar12;
  } while (iVar12 < 8);
  iVar13 = 1;
  iVar15 = 0;
  do {
    iVar12 = iVar13;
    if (iVar12 < 8) {
      psVar11 = asStack_80 + iVar15 * 2;
      psVar18 = asStack_80 + iVar12 * 2;
      iVar13 = iVar12;
      do {
        iVar13 = iVar13 + 1;
        if ((*(int16_t*)rtguest::G((uint32_t)psVar11)) < (*(int16_t*)rtguest::G((uint32_t)psVar18))) {
          (*(uint32_t*)rtguest::G((uint32_t)uStack_60)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)psVar11));
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)psVar11)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)psVar18));
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)psVar18)) = (*(uint32_t*)rtguest::G((uint32_t)uStack_60));
        }
        psVar18 = psVar18 + 0x4;  // T* element step
      } while (iVar13 < 8);
    }
    iVar13 = iVar12 + 1;
    iVar15 = iVar12;
  } while (iVar12 < 7);
  iVar12 = (int)param_1;
  uVar7 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 8)));
  iVar15 = 0x4d;
  psVar18 = psVar8;
  iVar13 = 0;
  do {
    if (((*(int16_t*)rtguest::G((uint32_t)psVar18)) != 0) || (uVar16 = 6, (uVar7 & 0x20) == 0)) {
      uVar16 = 0;
    }
    iVar17 = iVar13 + 1;
    if (iVar13 << 3 < (int)uVar7) {
      RT_CALLRA(0x236a60, guest::uiDrawText(0x50,iVar15,(*(uint32_t*)rtguest::G((uint32_t)(0x29fd30u + (uint32_t)(iVar13) * 4))),uVar16));
      uVar5 = RT_CALLRA(0x236a78, guest::uiDrawText(0xaa,iVar15,0x333850,uVar16));
      uVar6 = RT_CALLRA(0x236a8c, guest::GetPlayerScoreboardPtr((*(int16_t*)rtguest::G((uint32_t)psVar18)) * 3));
      RT_CALLRA(0x236aa0, guest::uiDrawText(uVar5,iVar15,uVar6,uVar16));
      RT_CALLRA(0x236ab8, guest::uiDrawNumber10(0x212,iVar15 << 1,(*(int16_t*)rtguest::G((uint32_t)asStack_80 + (uint32_t)(iVar13 * 2) * 2)),uVar16));
      uVar7 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 8)));
    }
    psVar18 = psVar18 + 0x4;  // T* element step
    iVar15 = iVar15 + 0xc;
    iVar13 = iVar17;
  } while (iVar17 < 8);
  if ((int)uVar7 < 0x40) {
    if ((uVar7 & 7) != 0) {
      return 0;
    }
    RT_CALLRA(0x236aec, guest::PlayUiSound(0x22));
    return 0;
  }
  iVar13 = (int)param_2;
  if ((*(int32_t*)rtguest::G(0x1782a70)) == 6) {
    iVar15 = 0;
    if ((*(int16_t*)rtguest::G((uint32_t)asStack_80 + (uint32_t)(1) * 2)) != 0) {
      iVar15 = 1;
      while ((psVar8 = psVar8 + 0x4, iVar15 < 8 && ((*(int16_t*)rtguest::G((uint32_t)psVar8)) != 0))) {
        iVar15 = iVar15 + 1;
      }
    }
    RT_CALLRA(0x236b5c, guest::uiDrawPanelCells(10,0xe,0x14,0x11,0));
    if (iVar15 < 6) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x20))) =
           (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0029fcb8 + ((uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x10))) + 3))) * 6 + iVar15) * 4)));
      RT_CALLRA(0x236bb0, guest::uiDrawTextCell(0xb,0xf,0x2ec480,0));
      RT_CALLRA(0x236bd0, guest::uiDrawTextCell(0xb,0x10,(*(uint32_t*)rtguest::G((uint32_t)(0x29fd30u + (uint32_t)(iVar15) * 4))),0));
      RT_CALLRA(0x236be8, guest::uiDrawTextCell(0xd,0x10,0x333838,0));
      RT_CALLRA(0x236c1c, guest::uiDrawNumber10(0x19a,0x18a,
                     (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0029fcb8 +
                      ((uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x10))) + 3))) * 6 + iVar15) * 4))),0));
      RT_CALLRA(0x236c2c, guest::DrawScoreAndRemaining(-(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x20))),param_2));
      uVar4 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x30)));
    }
    else {
      uVar4 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x30)));
    }
  }
  else {
    uVar4 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x30)));
  }
  if ((uVar4 & 0x40) == 0) {
    return 0;
  }
  (*(int32_t*)rtguest::G(0x1782a70)) = (*(int32_t*)rtguest::G(0x1782a70)) + 1;
  RT_CALLRA(0x236c54, guest::PlayUiSound(0x1a));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x28))) | 0x400000;
  if ((*(int32_t*)rtguest::G(0x1782a70)) == 7) {
    RT_CALLRA(0x236c84, guest::SpendTimerBudget(-(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x20))),0));
    (*(int32_t*)rtguest::G(0x1782a70)) = 0;
    if ((*(int16_t*)rtguest::G((uint32_t)asStack_80 + (uint32_t)(1) * 2)) == 0) {
      iVar13 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 4))) = 7;
      goto LAB_00236cf0;
    }
    iVar13 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x18)));
    uVar9 = 6;
  }
  else {
    if ((*(uint8_t*)rtguest::G(0x178700a)) == -1) {
      iVar13 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 4))) = 0;
      goto LAB_00236cf0;
    }
    iVar13 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x18)));
    uVar9 = 3;
  }
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x24))) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 4))) = uVar9;
LAB_00236cf0:
  RT_CALLRA(0x236cf0, guest::gs_tasks_kill(param_1));
  return 0;
}


RT_GUEST_FN(0x00236858, guest::UpdateRankingsAndAdvance)

}  // namespace guest
