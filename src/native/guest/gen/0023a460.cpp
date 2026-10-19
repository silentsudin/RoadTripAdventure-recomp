// guest:0x0023a460 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t HandleAmountAdjuster(uint32_t param_1, uint32_t param_2, ...) {
  byte bVar1;
  int iVar2;
uint32_t pcVar3;
  ushort uVar4;
  uint uVar5;
  long lVar6;
  ushort uVar7;
  undefined4 uVar8;
  int iVar9;
  int iVar10;
  
  iVar9 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8))) == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20))) = 1000;
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) = 1;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27))) = 3;
    return 0;
  }
  uiDrawPanelCells(10,7,0x13,9,2);
  iVar10 = (int)param_2;
  uVar7 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x34)));
  uVar4 = uVar7 & 0x2000;
  if ((uVar7 & 0x8000) != 0) {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27))) < 5) {
      PlayUiSound(0x1d);
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27))) + '\x01';
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) * 10;
      uVar4 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x34))) & 0x2000;
    }
    else {
      uVar4 = uVar7 & 0x2000;
    }
  }
  if (uVar4 == 0) {
    uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8)));
  }
  else if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27))) < 4) {
    uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8)));
  }
  else {
    PlayUiSound(0x1d);
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27))) + -1;
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) / 10;
    uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8)));
  }
  if ((uVar5 & 0x10) != 0) {
    uiDrawIcon20((uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27))) * -0x10 + 0x138,0xb2,2);
    uiDrawIcon20((uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27))) * -0x10 + 0x138,0xe2,3);
  }
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20)));
  for (uVar5 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27))); uVar5 != 0; uVar5 = uVar5 - 1) {
    iVar2 = iVar2 / 10;
  }
  (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x26))) = (char)(iVar2 % 10);
  uVar7 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x34)));
  if ((uVar7 & 0x1000) != 0) {
    if ((iVar2 % 10 & 0xffU) < 9) {
      PlayUiSound(0x1d);
      uVar7 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x34)));
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20))) =
           (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20))) + ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) * 0x7c + (int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2)))) * 8;
    }
    else if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x30))) & 0x1000) != 0) {
      PlayUiSound(0x1d);
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x26))) = 0;
      uVar7 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x34)));
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20))) + (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) * -9000;
    }
  }
  if ((uVar7 & 0x4000) == 0) {
    uVar8 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20)));
  }
  else {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x26))) == '\0') {
      if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x30))) & 0x4000) == 0) {
        uVar8 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20)));
        goto LAB_0023a6e0;
      }
      PlayUiSound(0x1d);
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20))) + (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) * 9000;
    }
    else {
      PlayUiSound(0x1d);
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20))) =
           (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20))) + ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) * 0x7c + (int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2)))) * -8
      ;
    }
    uVar8 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20)));
  }
LAB_0023a6e0:
  uiDrawNumber16(0x14a,0x65,uVar8,10);
  uiDrawTextCell(0x10,8,0x177fd9f,10);
  if ((*(int8_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x1c))))) == '\0') {
    lVar6 = DrawAltScoreAndRemaining((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20))),param_2);
  }
  else {
    lVar6 = DrawAltScoreAndRemaining(-(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20))),param_2);
  }
  if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x30))) & 0x40) == 0) {
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x30))) & 0x10) == 0) {
      return 0;
    }
    iVar10 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x18)));
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x1c))) + 1)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 4))) = (uint32_t)bVar1;
    PlayUiSound(0x1b);
    gs_tasks_kill(param_1);
    return 0;
  }
  pcVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x1c)));
  if ((*(int8_t*)rtguest::G((uint32_t)pcVar3)) == '\0') {
    if (lVar6 == -1) {
      PlayUiSound(0x1a);
      iVar10 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x18)));
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x1c))) + 3)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 4))) = (uint32_t)bVar1;
    }
    else {
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20))) == 0) {
        bVar1 = (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(1) * sizeof(int8_t)));
      }
      else {
        bVar1 = (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(2) * sizeof(int8_t)));
      }
      iVar10 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 4))) = (uint32_t)bVar1;
      SpendTimerBudget((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20))),0);
      (*(int32_t*)rtguest::G(0x1782a6c)) = (*(int32_t*)rtguest::G(0x1782a6c)) + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20)));
    }
  }
  else if (lVar6 == 1) {
    PlayUiSound(0x1a);
    iVar10 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x18)));
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x1c))) + 4)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 4))) = (uint32_t)bVar1;
  }
  else if ((*(int32_t*)rtguest::G(0x1782a6c)) < (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20)))) {
    PlayUiSound(0x1a);
    iVar10 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x18)));
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x1c))) + 3)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 4))) = (uint32_t)bVar1;
  }
  else {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20))) == 0) {
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(1) * sizeof(int8_t)));
    }
    else {
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(2) * sizeof(int8_t)));
    }
    iVar10 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 4))) = (uint32_t)bVar1;
    SpendTimerBudget(-(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20))),0);
    (*(int32_t*)rtguest::G(0x1782a6c)) = (*(int32_t*)rtguest::G(0x1782a6c)) - (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20)));
  }
  gs_tasks_kill(param_1);
  return 0;
}


RT_GUEST_FN(0x0023a460, guest::HandleAmountAdjuster)

}  // namespace guest
