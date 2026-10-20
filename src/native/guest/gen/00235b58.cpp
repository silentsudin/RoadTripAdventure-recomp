// guest:0x00235b58 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawRaceResultsScreen(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  byte bVar1;
  ushort uVar2;
  char cVar3;
  undefined4 uVar4;
  undefined8 uVar5;
  int iVar6;
  int iVar7;
  
  iVar6 = (int)param_1;
  iVar7 = (int)param_2;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) != 0) {
    uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)));
    goto LAB_00235ca4;
  }
  if (4 < (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x20)))) {
    UpdateRaceRecords(param_2);
    uiFade(0xffffffff80000000);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) | 0x400000;
    uVar4 = GetOrSetPresidentRaceResult(0xffffffffffffffff);
    iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) = uVar4;
    gs_tasks_kill(param_1);
    return 0;
  }
  uVar5 = getExtraPacketArea(0x1f0);
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) = (int)uVar5;
  InitScreenLayout(uVar5);
  if (((((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x10))) + 3))) & 8) == 0) || ((*(int8_t*)rtguest::G(0x177fdd6)) == '\0')) ||
     ((*(uint32_t*)rtguest::G(0x178321e)) == '\0')) {
    if ((*(uint8_t*)rtguest::G(0x178700a)) < (*(uint32_t*)rtguest::G(0x178700e))) {
      cVar3 = '\x15';
      goto LAB_00235c18;
    }
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26)));
  }
  else {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x27))) = 1;
    cVar3 = ((-(*(uint8_t*)rtguest::G(0x1787012)) - (*(uint8_t*)rtguest::G(0x178700a))) - (*(uint32_t*)rtguest::G(0x178701a))) + '\x12';
LAB_00235c18:
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26))) = cVar3;
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26)));
  }
  if (bVar1 < 0xb) {
    GetOrSetPresidentRaceResult(1);
    uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)));
  }
  else {
    GetOrSetPresidentRaceResult(2);
    uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)));
  }
LAB_00235ca4:
  vifDirect(uVar4,0x1f);
  uiDrawText(0xe6,0x1b,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x2be398 + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x10)))))) * 4)),0);
  uiDrawPanelCells(2,3,0x1c,0xb,0);
  uiDrawTextCell(4,4,0x2ec430,0);
  uiDrawTextCell(0xd,4,0x3337f8,0);
  uiDrawTextCell(0x10,4,0x333800,0);
  uiDrawTextCell(0x14,4,0x333808,0);
  uiDrawTextCell(0x17,4,0x2ec440,0);
  DrawStandingsEntry(0,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x20))),0);
  DrawStandingsEntry(0,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x20))),1);
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x27))) != '\0') {
    DrawStandingsEntry(1,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x20))),0);
    DrawStandingsEntry(2,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x20))),0);
    DrawStandingsEntry(1,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x20))),1);
    DrawStandingsEntry(2,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x20))),1);
  }
  uiDrawPanelCells(9,0xb,0x15,0x10,0);
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x27))) != '\0') {
    uiDrawTextCell(0xb,0xc,0x2ec450,0);
    uiDrawNumber16(300,0xa1,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26))),0);
    uiDrawTextCell(0xf,0xd,0x333810,0);
    uiDrawNumber16(0x172,0xa1,0x15 - (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26))),0);
  }
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26))) < 0xb) {
    uVar5 = 6;
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) & 0x10) != 0) {
      uVar5 = 0;
    }
    uiDrawTextCell(0xb,0xe,0x2ec470,uVar5);
    uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x30)));
  }
  else {
    uVar5 = 6;
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) & 0x10) != 0) {
      uVar5 = 0;
    }
    uiDrawTextCell(0xb,0xe,0x2ec460,uVar5);
    uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x30)));
  }
  if (((uVar2 | (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x32)))) & 0x40) != 0) {
    UpdateRaceRecords(param_2);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) | 0x400000;
    uVar4 = GetOrSetPresidentRaceResult(0xffffffffffffffff);
    iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) = uVar4;
    gs_tasks_kill(param_1);
  }
  return 0;
}


RT_GUEST_FN(0x00235b58, guest::DrawRaceResultsScreen)

}  // namespace guest
