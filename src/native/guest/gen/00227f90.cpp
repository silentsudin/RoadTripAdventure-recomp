// guest:0x00227f90 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

void DrawSaveSlotEntry(uint32_t param_1, uint64_t param_2, uint32_t param_3) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_3);  // walk: = param_3
    rtguest::setS32(2, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(3, (uint32_t)param_1);  // walk: = param_1
  }
uint32_t pcVar1;
  int iVar2;
  
  iVar2 = (int)param_2;
  RT_CALLRA(0x227fc8, guest::uiDrawPanelCells(8,param_2,0x16,iVar2 + 4 + (uint32_t)(param_3 != 0),1));
  if (param_3 != 0) {
    if (param_3 == 1) {
      pcVar1 = 0x2eb4a0u;
    }
    else {
      pcVar1 = 0x2eb4b0u;
    }
    uiDrawTextCell(0xc,iVar2 + 1,pcVar1,10);
    iVar2 = iVar2 + 1;
  }
  uiDrawTextCell(9,iVar2 + 1,param_1 + 0x62c,0);
  uiDrawTextCell(10,iVar2 + 2,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x2be438 + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x12dc))) * 2) * 4)),0);
  RT_CALLRA(0x228078, guest::uiDrawNumber10(0xfa,iVar2 * 0x18 + 0x52,(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x658))) / 0x649,0));
  uiDrawTextCell(0xc,iVar2 + 3,0x333610,0);
  RT_CALLRA(0x2280a4, guest::uiDrawNumber16(0x186,iVar2 * 0xc + 0x29,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x654))),0));
  uiDrawTextCell(0x13,iVar2 + 3,param_1 + 0x63f,0);
  return;
}


RT_GUEST_FN(0x00227f90, guest::DrawSaveSlotEntry)

}  // namespace guest
