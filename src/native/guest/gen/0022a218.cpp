// guest:0x0022a218 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t FormatMemoryCardTask(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
  long lVar1;
  int iVar2;
uint32_t auStack_30 = rtguest::allocFrame(4);
uint32_t auStack_2c = rtguest::allocFrame(12);
  
  RT_CALLRA(0x22a23c, guest::uiDrawPanelCells(9,8,0x15,10,0));
  RT_CALLRA(0x22a254, guest::uiDrawTextCell(10,9,0x2eb7c8,0));
  iVar2 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 8))) == 0) {
    lVar1 = RT_CALLRA(0x22a270, guest::sceMcFormat((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x1c))),0));
    if (lVar1 != 0) {
      rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2eb7e0u)), lVar1);
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24))) = 1;
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = (int)lVar1;
      RT_CALLRA(0x22a2a4, guest::gs_tasks_kill(param_1));
    }
  }
  else {
    lVar1 = RT_CALLRA(0x22a2b8, guest::sceMcSync(1,auStack_30,auStack_2c));
    if (lVar1 == 1) {
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = (*(uint32_t*)rtguest::G((uint32_t)auStack_2c + (uint32_t)(0) * 4));
      RT_CALLRA(0x22a2dc, guest::gs_tasks_kill(param_1));
    }
  }
  return 0;
}


RT_GUEST_FN(0x0022a218, guest::FormatMemoryCardTask)

}  // namespace guest
