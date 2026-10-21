// guest:0x00235b58 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawRaceResultsScreen(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
  }
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
    RT_CALLRA(0x235c50, guest::UpdateRaceRecords(param_2));
    RT_CALLRA(0x235c58, guest::uiFade(0xffffffff80000000));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) | 0x400000;
    uVar4 = RT_CALLRA(0x235c2c, guest::GetOrSetPresidentRaceResult(0xffffffffffffffff));
    iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) = uVar4;
    gs_tasks_kill(param_1);
    return 0;
  }
  uVar5 = RT_CALLRA(0x235b88, guest::getExtraPacketArea(0x1f0));
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) = (int)uVar5;
  RT_CALLRA(0x235b94, guest::InitScreenLayout(uVar5));
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
    RT_CALLRA(0x235c40, guest::GetOrSetPresidentRaceResult(1));
    uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)));
  }
  else {
    RT_CALLRA(0x235c70, guest::GetOrSetPresidentRaceResult(2));
    uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)));
  }
LAB_00235ca4:
  RT_CALLRA(0x235ca4, guest::vifDirect(uVar4,0x1f));
  RT_CALLRA(0x235ccc, guest::uiDrawText(0xe6,0x1b,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x2be398 + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x10)))))) * 4)),0));
  RT_CALLRA(0x235ce4, guest::uiDrawPanelCells(2,3,0x1c,0xb,0));
  RT_CALLRA(0x235cfc, guest::uiDrawTextCell(4,4,0x2ec430,0));
  RT_CALLRA(0x235d14, guest::uiDrawTextCell(0xd,4,0x3337f8,0));
  RT_CALLRA(0x235d2c, guest::uiDrawTextCell(0x10,4,0x333800,0));
  RT_CALLRA(0x235d44, guest::uiDrawTextCell(0x14,4,0x333808,0));
  RT_CALLRA(0x235d5c, guest::uiDrawTextCell(0x17,4,0x2ec440,0));
  RT_CALLRA(0x235d6c, guest::DrawStandingsEntry(0,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x20))),0));
  RT_CALLRA(0x235d7c, guest::DrawStandingsEntry(0,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x20))),1));
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x27))) != '\0') {
    RT_CALLRA(0x235d94, guest::DrawStandingsEntry(1,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x20))),0));
    RT_CALLRA(0x235da4, guest::DrawStandingsEntry(2,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x20))),0));
    RT_CALLRA(0x235db4, guest::DrawStandingsEntry(1,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x20))),1));
    RT_CALLRA(0x235dc4, guest::DrawStandingsEntry(2,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x20))),1));
  }
  RT_CALLRA(0x235ddc, guest::uiDrawPanelCells(9,0xb,0x15,0x10,0));
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x27))) != '\0') {
    RT_CALLRA(0x235dfc, guest::uiDrawTextCell(0xb,0xc,0x2ec450,0));
    RT_CALLRA(0x235e10, guest::uiDrawNumber16(300,0xa1,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26))),0));
    RT_CALLRA(0x235e28, guest::uiDrawTextCell(0xf,0xd,0x333810,0));
    RT_CALLRA(0x235e44, guest::uiDrawNumber16(0x172,0xa1,0x15 - (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26))),0));
  }
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26))) < 0xb) {
    uVar5 = 6;
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) & 0x10) != 0) {
      uVar5 = 0;
    }
    RT_CALLRA(0x235e74, guest::uiDrawTextCell(0xb,0xe,0x2ec470,uVar5));
    uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x30)));
  }
  else {
    uVar5 = 6;
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) & 0x10) != 0) {
      uVar5 = 0;
    }
    RT_CALLRA(0x235ea0, guest::uiDrawTextCell(0xb,0xe,0x2ec460,uVar5));
    uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x30)));
  }
  if (((uVar2 | (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x32)))) & 0x40) != 0) {
    RT_CALLRA(0x235ec0, guest::UpdateRaceRecords(param_2));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) | 0x400000;
    uVar4 = RT_CALLRA(0x235ed8, guest::GetOrSetPresidentRaceResult(0xffffffffffffffff));
    iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) = uVar4;
    gs_tasks_kill(param_1);
  }
  return 0;
}


RT_GUEST_FN(0x00235b58, guest::DrawRaceResultsScreen)

}  // namespace guest
