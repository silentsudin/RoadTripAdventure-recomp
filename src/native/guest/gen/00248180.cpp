// guest:0x00248180 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateRaceResultsMenu(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  char cVar1;
  ushort uVar2;
  int iVar3;
  bool bVar4;
  int iVar5;
  int iVar6;
  
  iVar6 = (int)param_2;
  uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x30)));
  if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 10))) & 2) != 0) {
    uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x32))) | uVar2;
  }
  iVar5 = (int)param_1;
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8)));
  if (iVar3 == 0) {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 10))) == 1) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28))) =
           0x2be158 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c))) * 0x10;
    }
    else {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28))) =
           0x2be1d8 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c))) * 0x10;
    }
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25)));
  }
  else {
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25)));
  }
  if (cVar1 == '\0') {
    if (iVar3 < 0x10) {
      uiSetTexa(iVar3 << 3);
      bVar4 = false;
    }
    else {
      bVar4 = false;
    }
  }
  else {
    bVar4 = false;
    if (cVar1 == '\x02') {
      if (iVar3 < 0x10) {
        uiSetTexa(iVar3 * -8 + 0x80);
        bVar4 = false;
      }
      else {
        gs_tasks_kill(param_1);
        bVar4 = true;
      }
    }
  }
  if (bVar4) {
    return 0;
  }
  DrawCarouselStatText((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28))),param_2);
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 10))) == '\x01') {
    DrawCarouselSlotPanel(param_1,param_2);
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25)));
  }
  else {
    DrawTwoPlayerResults(param_1,param_2,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28))));
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25)));
  }
  if (cVar1 != '\0') {
    return 0;
  }
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) < 0x11) {
    uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x30)));
  }
  else {
    if ((uVar2 & 0x40) != 0) {
      PlayUiSound(0x1a);
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) | 0x400000;
      gs_tasks_kill(param_1);
      return 0;
    }
    uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x30)));
  }
  if ((uVar2 & 0x10) == 0) {
    if ((((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x32))) & 0x10) == 0) || (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 10))) & 2) == 0))
    goto LAB_00248364;
    iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x18)));
  }
  else {
    iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x18)));
  }
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) = 0xffffffff;
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25))) = 2;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) = 0;
  PlayUiSound(0x1b);
LAB_00248364:
  uiDrawIcon3(0x1fe,0x20,0);
  uiDrawIcon3(0x1a4,0x20,1);
  return 0;
}


RT_GUEST_FN(0x00248180, guest::UpdateRaceResultsMenu)

}  // namespace guest
