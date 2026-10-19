// guest:0x00236858 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateRankingsAndAdvance(uint32_t param_1, uint32_t param_2, ...) {
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
uint32_t asStack_80 = rtguest::allocFrame(32);
  
  uiDrawPanelCells(2,4,0x1c,0xe,0);
  uiDrawTextCell(3,5,0x333848,0);
  uiDrawTextCell(10,5,0x2ec4c0,0);
  uiDrawTextCell(0x15,5,0x2ec4d0,0);
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
        psVar18 = psVar18 + 2;
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
      uiDrawText(0x50,iVar15,(*(uint32_t*)rtguest::G((uint32_t)(0x29fd30u + (uint32_t)(iVar13) * 4))),uVar16);
      uVar5 = uiDrawText(0xaa,iVar15,0x333850,uVar16);
      uVar6 = GetPlayerScoreboardPtr((*(int16_t*)rtguest::G((uint32_t)psVar18)) * 3);
      uiDrawText(uVar5,iVar15,uVar6,uVar16);
      uiDrawNumber10(0x212,iVar15 << 1,(*(int16_t*)rtguest::G((uint32_t)asStack_80 + (uint32_t)(iVar13 * 2) * 2)),uVar16);
      uVar7 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 8)));
    }
    psVar18 = psVar18 + 2;
    iVar15 = iVar15 + 0xc;
    iVar13 = iVar17;
  } while (iVar17 < 8);
  if ((int)uVar7 < 0x40) {
    if ((uVar7 & 7) != 0) {
      return 0;
    }
    PlayUiSound(0x22);
    return 0;
  }
  iVar13 = (int)param_2;
  if ((*(int32_t*)rtguest::G(0x1782a70)) == 6) {
    iVar15 = 0;
    if ((*(int16_t*)rtguest::G((uint32_t)asStack_80 + (uint32_t)(1) * 2)) != 0) {
      iVar15 = 1;
      while ((psVar8 = psVar8 + 2, iVar15 < 8 && ((*(int16_t*)rtguest::G((uint32_t)psVar8)) != 0))) {
        iVar15 = iVar15 + 1;
      }
    }
    uiDrawPanelCells(10,0xe,0x14,0x11,0);
    if (iVar15 < 6) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x20))) =
           (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0029fcb8 + ((uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x10))) + 3))) * 6 + iVar15) * 4)));
      uiDrawTextCell(0xb,0xf,0x2ec480,0);
      uiDrawTextCell(0xb,0x10,(*(uint32_t*)rtguest::G((uint32_t)(0x29fd30u + (uint32_t)(iVar15) * 4))),0);
      uiDrawTextCell(0xd,0x10,0x333838,0);
      uiDrawNumber10(0x19a,0x18a,
                     (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0029fcb8 +
                      ((uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x10))) + 3))) * 6 + iVar15) * 4))),0);
      DrawScoreAndRemaining(-(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x20))),param_2);
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
  PlayUiSound(0x1a);
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x28))) | 0x400000;
  if ((*(int32_t*)rtguest::G(0x1782a70)) == 7) {
    SpendTimerBudget(-(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x20))),0);
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
  gs_tasks_kill(param_1);
  return 0;
}


RT_GUEST_FN(0x00236858, guest::UpdateRankingsAndAdvance)

}  // namespace guest
