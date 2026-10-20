// guest:0x00238a48 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawRaceSummaryRow(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  int iVar1;
  int iVar2;
  
  iVar2 = (int)param_1 * 0x10;
  iVar1 = 0x10;
  if ((param_2 & 1) == 0) {
    iVar1 = 2;
  }
  uiDrawTextCell(iVar1,2,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x2be398 + (uint32_t)((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x2bddd0 + (uint32_t)(iVar2) * 4))) * 4)),3);
  uiDrawTextCell(iVar1 + 7,3,(*(uint32_t*)rtguest::G((uint32_t)(0x29fdb0u + (uint32_t)((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x002bddd3u + (uint32_t)(iVar2) * 4))) * 4))),3);
  DrawShopRow(iVar1,4,param_1,0);
  DrawShopRow(iVar1,8,param_1,1);
  DrawShopRow(iVar1,0xc,param_1,2);
  return 0;
}


RT_GUEST_FN(0x00238a48, guest::DrawRaceSummaryRow)

}  // namespace guest
