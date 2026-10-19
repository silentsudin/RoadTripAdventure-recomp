// guest:0x0022e960 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawTextListRows(uint32_t param_1, uint32_t param_2, ...) {
  long lVar1;
  int iVar2;
  int iVar3;
  
  iVar3 = 0;
  while ((iVar3 < 4 && (lVar1 = GetNthStringByCategory(param_1,param_2,iVar3), lVar1 != 0))) {
    if (iVar3 == 0) {
      uiDrawPanelCells(0x11,0xc,0x1e,0x11,1);
    }
    iVar2 = iVar3 + 0xd;
    iVar3 = iVar3 + 1;
    uiDrawTextCell(0x12,iVar2,lVar1,10);
  }
  return 0;
}


RT_GUEST_FN(0x0022e960, guest::DrawTextListRows)

}  // namespace guest
