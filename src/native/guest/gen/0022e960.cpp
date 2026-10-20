// guest:0x0022e960 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

void DrawTextListRows(uint64_t param_1, uint64_t param_2) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
  long lVar1;
  int iVar2;
  int iVar3;
  
  iVar3 = 0;
  while ((iVar3 < 4 && (lVar1 = GetNthStringByCategory(param_1,param_2,iVar3), lVar1 != 0))) {
    if (iVar3 == 0) {
      RT_CALLRA(0x22e990, guest::uiDrawPanelCells(0x11,0xc,0x1e,0x11,1));
    }
    iVar2 = iVar3 + 0xd;
    iVar3 = iVar3 + 1;
    RT_CALLRA(0x22e9a8, guest::uiDrawTextCell(0x12,iVar2,lVar1,10));
  }
  return;
}


RT_GUEST_FN(0x0022e960, guest::DrawTextListRows)

}  // namespace guest
