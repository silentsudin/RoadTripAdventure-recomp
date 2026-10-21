// guest:0x0026cf20 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t BrowseItemsMenuTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
  }
  int iVar1;
  byte bVar2;
  int iVar3;
  int iVar4;
  
  RT_CALLRA(0x26cf34, guest::creditsScene());
  iVar3 = (int)param_1;
  RT_CALLRA(0x26cf44, guest::DrawRaceSummaryRow((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x26))),0,param_2));
  RT_CALLRA(0x26cf58, guest::DrawRaceSummaryRow((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x26))) + 1,1,param_2));
  bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x26)));
  iVar4 = (int)param_2;
  if ((bVar2 < 0x18) && (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x34))) & 0x2000) != 0)) {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x26))) = bVar2 + 2;
    bVar2 = bVar2 + 2;
  }
  if (bVar2 == 0) {
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8)));
  }
  else {
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x34))) & 0x8000) != 0) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x26))) = bVar2 - 2;
    }
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8)));
  }
  if ((iVar1 != 0) && (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x30))) & 0x10) != 0)) {
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = 0xffffffff;
    gs_tasks_kill(param_1);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x0026cf20, guest::BrowseItemsMenuTask)

}  // namespace guest
