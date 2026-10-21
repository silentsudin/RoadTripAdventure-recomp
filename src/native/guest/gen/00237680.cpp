// guest:0x00237680 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawShopRow(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x38, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x28, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x30, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x40) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x44) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_4);  // walk: = param_4
    rtguest::setS32(2, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(3, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(4, (uint32_t)param_3);  // walk: = param_3
    rtguest::gS[7] = 0x000000000177f760ull;  // walk const
  }
  int iVar1;
uint32_t puVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  iVar3 = (int)param_4;
  iVar4 = (int)param_1;
  iVar6 = iVar4 + 1;
  iVar5 = (int)param_2;
  puVar2 = 0x0177fd8c;
  if ((*(uint32_t*)rtguest::G((uint32_t)0x1780750 + (uint32_t)(param_3 + iVar3 * 0x1a) * 4)) != -1) {
    if (param_4 != 0) {
      puVar2 = (uint32_t)0x0;
      iVar1 = (param_3 * 2 + iVar3) * 2;
      if ((*(uint32_t*)rtguest::G((uint32_t)0x1782b1e + (uint32_t)(iVar1) * 4)) != '\0') {
        puVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((char)(*(uint32_t*)rtguest::G((uint32_t)0x1782b1f + (uint32_t)(iVar1) * 4)) * 0x10 +
                   (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x2c22c8 + (char)(*(uint32_t*)rtguest::G((uint32_t)0x1782b1e + (uint32_t)(iVar1) * 4)) * 4))) + 0xc)));
      }
    }
    RT_CALLRA(0x23774c, guest::uiDrawTextCell(param_1,param_2,puVar2,3));
    param_3 = param_3 + iVar3 * 0x1a;
    RT_CALLRA(0x237778, guest::uiDrawTextCell(iVar6,iVar5 + 1,0x2ec4f0,3));
    iVar3 = param_3 * 4;
    iVar1 = iVar4 * 0x14 + 0xaa;
    RT_CALLRA(0x2377b8, guest::uiDrawTextCell(iVar4 + 10,iVar5 + 1,(*(uint32_t*)rtguest::G((uint32_t)(0x29fd30u + (uint32_t)((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x01780750u + (uint32_t)(param_3) * 4))) * 4))),3));
    RT_CALLRA(0x2377d0, guest::uiDrawTextCell(iVar6,iVar5 + 2,0x2ec500,3));
    uiDrawTime(iVar1,iVar5 * 0xc + 0x1d,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017807a0 + iVar3))),3);
    RT_CALLRA(0x237800, guest::uiDrawTextCell(iVar6,iVar5 + 3,0x2ec510,3));
    uiDrawTime(iVar1,iVar5 * 0xc + 0x29,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017808d8 + iVar3))),3);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x00237680, guest::DrawShopRow)

}  // namespace guest
