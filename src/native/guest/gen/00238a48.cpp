// guest:0x00238a48 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawRaceSummaryRow(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
    rtguest::gS[0] = 0x0000000000000010ull;  // walk const
    rtguest::setS32(2, (uint32_t)param_1);  // walk: = param_1
  }
  int iVar1;
  int iVar2;
  
  iVar2 = (int)param_1 * 0x10;
  iVar1 = 0x10;
  if ((param_2 & 1) == 0) {
    iVar1 = 2;
  }
  RT_CALLRA(0x238a9c, guest::uiDrawTextCell(iVar1,2,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x2be398 + (uint32_t)((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x2bddd0 + (uint32_t)(iVar2) * 4))) * 4)),3));
  RT_CALLRA(0x238ac0, guest::uiDrawTextCell(iVar1 + 7,3,(*(uint32_t*)rtguest::G((uint32_t)(0x29fdb0u + (uint32_t)((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x002bddd3u + (uint32_t)(iVar2) * 4))) * 4))),3));
  DrawShopRow(iVar1,4,param_1,0);
  DrawShopRow(iVar1,8,param_1,1);
  DrawShopRow(iVar1,0xc,param_1,2);
  return 0;
}


RT_GUEST_FN(0x00238a48, guest::DrawRaceSummaryRow)

}  // namespace guest
