// guest:0x002364b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawLicenseResultsScreen(uint32_t param_1, uint32_t param_2, ...) {
  byte bVar1;
  uint uVar2;
  char cVar3;
  long lVar4;
  int iVar5;
  undefined4 uVar6;
uint32_t pcVar7;
  undefined8 uVar8;
  int iVar9;
uint32_t puVar10;
  
  iVar9 = (int)param_1;
  vifDirect((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28))),0x1f);
  puVar10 = (uint32_t)param_2;
  uiDrawText(0xe6,0x1b,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x2be398 + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar10 + 2)))))) * 4)),0);
  uiDrawPanelCells(2,3,0x1c,0xb,0);
  uiDrawTextCell(4,4,0x2ec430,0);
  uiDrawTextCell(0xd,4,0x333840,0);
  uiDrawTextCell(0x13,4,0x333808,0);
  uiDrawTextCell(0x17,4,0x2ec440,0);
  DrawPlayerResults(0,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(puVar10 + 4))),(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8))) + 4U & 0x10);
  if ((*(int8_t*)rtguest::G(0x177fdd4)) != '\0') {
    DrawPlayerResults(1,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(puVar10 + 4))),(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8))) + 8U & 0x10);
  }
  if ((*(int8_t*)rtguest::G(0x177fdd6)) != '\0') {
    DrawPlayerResults(2,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(puVar10 + 4))),(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8))) + 0xcU & 0x10);
  }
  iVar5 = 0;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20))) != 0) {
    uiDrawPanelCells(10,0xb,0x14,0x11,0);
    uiDrawTextCell(0xb,0xc,0x2ec480,0);
    DrawLicenseList(param_2);
    bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27)));
    if (bVar1 == 2) {
      uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8)));
      pcVar7 = 0x2ec4a0u;
LAB_00236688:
      uVar8 = 6;
      if ((uVar2 & 0x10) != 0) {
        uVar8 = 0;
      }
      uiDrawTextCell(0xb,0x10,pcVar7,uVar8);
      iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20)));
    }
    else if (bVar1 < 3) {
      if (bVar1 == 1) {
        uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8)));
        pcVar7 = 0x2ec490u;
        goto LAB_00236688;
      }
      iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20)));
    }
    else if (bVar1 == 3) {
      uVar8 = 6;
      if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8))) & 0x10) != 0) {
        uVar8 = 0;
      }
      uiDrawTextCell(0xb,0x10,0x2ec4b0,uVar8);
      iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20)));
    }
    else {
      iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20)));
    }
  }
  DrawScoreAndRemaining(-iVar5,param_2);
  if (((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8))) == 0) || (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(puVar10 + 6))) & 0x40) == 0)) {
    return 0;
  }
  SpendTimerBudget(-(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20))),0);
  if ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27))) != '\0') {
    (*(int8_t*)rtguest::G(0x177fdb1)) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27)));
  }
  UpdateRaceRecords(param_2);
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(puVar10 + 4))) == '\x18') {
    gs_stamps_award_function(0x31);
    SetProgressFlagAndQueue(5);
  }
  if ((*(uint8_t*)rtguest::G(0x178700a)) < 6) {
    lVar4 = TestStateBitFc78(0x28);
    if (lVar4 == 0) {
      cVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27)));
    }
    else {
      lVar4 = gs_stamps_award_function(0x5f);
      if (lVar4 == 0) {
        SubmitMessage31((int)(*(uint64_t*)rtguest::G((uint32_t)puVar10)));
        cVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27)));
      }
      else {
        AppendQueuedByte(0x1e);
        cVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27)));
      }
    }
  }
  else {
    cVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27)));
  }
  uVar6 = 5;
  if (cVar3 == '\0') {
    if ((*(uint8_t*)rtguest::G(0x178700a)) == 0) {
      iVar9 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x18)));
      uVar6 = 4;
    }
    else if ((*(uint8_t*)rtguest::G(0x178700a)) < 6) {
      iVar9 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x18)));
      uVar6 = 3;
    }
    else {
      uVar6 = 2;
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20))) == 0) {
        iVar9 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x18)));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24))) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 4))) = 1;
        goto LAB_00236808;
      }
      iVar9 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x18)));
    }
  }
  else {
    iVar9 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x18)));
  }
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24))) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 4))) = uVar6;
LAB_00236808:
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar10 + 5))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar10 + 5))) | 0x400000;
  gs_tasks_kill(param_1);
  return 0;
}


RT_GUEST_FN(0x002364b0, guest::DrawLicenseResultsScreen)

}  // namespace guest
