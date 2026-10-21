// guest:0x00248180 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateRaceResultsMenu(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(1, (uint32_t)param_2);  // walk: = param_2
  }
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
      RT_CALLRA(0x248214, guest::uiSetTexa(iVar3 << 3));
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
        RT_CALLRA(0x248240, guest::uiSetTexa(iVar3 * -8 + 0x80));
        bVar4 = false;
      }
      else {
        RT_CALLRA(0x248250, guest::gs_tasks_kill(param_1));
        bVar4 = true;
      }
    }
  }
  if (bVar4) {
    return 0;
  }
  RT_CALLRA(0x248268, guest::DrawCarouselStatText((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28))),param_2));
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 10))) == '\x01') {
    RT_CALLRA(0x248284, guest::DrawCarouselSlotPanel(param_1,param_2));
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25)));
  }
  else {
    RT_CALLRA(0x24829c, guest::DrawTwoPlayerResults(param_1,param_2,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28)))));
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
      RT_CALLRA(0x2482cc, guest::PlayUiSound(0x1a));
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) | 0x400000;
      RT_CALLRA(0x2482f4, guest::gs_tasks_kill(param_1));
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
  RT_CALLRA(0x248354, guest::PlayUiSound(0x1b));
LAB_00248364:
  RT_CALLRA(0x248364, guest::uiDrawIcon3(0x1fe,0x20,0));
  RT_CALLRA(0x248374, guest::uiDrawIcon3(0x1a4,0x20,1));
  return 0;
}


RT_GUEST_FN(0x00248180, guest::UpdateRaceResultsMenu)

}  // namespace guest
