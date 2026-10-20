// guest:0x00247bf0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawCarouselSlotPanel(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x28) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x2c) = 0x00000000u;
  }
  undefined8 uVar1;
  int iVar2;
  
  RT_CALLRA(0x247c24, guest::uiDrawPanelCells(10,10,0x1d,0x10,0));
  uVar1 = RT_CALLRA(0x247c30, guest::GetSlotDataPtr(0,0));
  uiDrawTextCell(0xb,0xb,uVar1,5);
  RT_CALLRA(0x247c50, guest::DrawCarouselStatBars((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))),param_2));
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
  uiDrawTextCell(0xb,0xd,0x3276b0,0);
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 10))) == '\x01') {
    RT_CALLRA(0x247ca4, guest::uiDrawTime(0x1d6,0xa1,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017802dc + iVar2 * 0xc))),0));
  }
  else {
    iVar2 = iVar2 * 0x20;
    RT_CALLRA(0x247cd8, guest::uiDrawTime(0x5a,0xa1,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801b4 + iVar2))),0));
    RT_CALLRA(0x247cec, guest::uiDrawTime(0x1c2,0xa1,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835fc + iVar2))),0));
  }
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
  uiDrawTextCell(0xb,0xe,0x3276c8,0);
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 10))) == '\x01') {
    RT_CALLRA(0x247d40, guest::uiDrawTime(0x1d6,0xad,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017802d8 + iVar2 * 0xc))),0));
  }
  else {
    iVar2 = iVar2 * 0x20;
    RT_CALLRA(0x247d6c, guest::uiDrawTime(0x5a,0xad,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801b0 + iVar2))),0));
    RT_CALLRA(0x247d80, guest::uiDrawTime(0x1c2,0xad,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835f8 + iVar2))),0));
  }
  uiDrawTextCell(0xb,0xf,0x3276d8,0);
  if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(0x017802e2 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) * 0xc))) != 0xff) {
    uiDrawTextCell(0x18,0xf,(*(uint32_t*)rtguest::G((uint32_t)(0x2a8ed0u + (uint32_t)((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(0x017802e2u + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) * 0xc)))) * 4))),0);
    return 0;
  }
  uiDrawTextCell(0x18,0xf,0x334490,0);
  return 0;
}


RT_GUEST_FN(0x00247bf0, guest::DrawCarouselSlotPanel)

}  // namespace guest
