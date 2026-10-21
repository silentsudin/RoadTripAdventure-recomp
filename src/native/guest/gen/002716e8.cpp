// guest:0x002716e8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

void textGridFillRectBlank(uint64_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5) {
  rtguest::FrameGuard rt_fg(144);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x70, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x30, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x80) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x84) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::gS[0] = 0x0000000000000000ull;  // walk const
    rtguest::gS[2] = 0x0000000000000001ull;  // walk const
    rtguest::setS32(5, (uint32_t)param_3);  // walk: = param_3
    rtguest::setS32(6, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(7, (uint32_t)param_1);  // walk: = param_1
  }
  int iVar1;
  uint uVar2;
  uint uVar3;
  
  uVar2 = 0;
  if (param_5 != 0) {
    do {
      uVar3 = 0;
      if (param_4 != 0) {
        iVar1 = param_2;
        do {
          RT_CALLRA(0x271750, guest::textBufferSetPixel16(param_1,iVar1,param_3 + uVar2,0x720));
          uVar3 = uVar3 + 1;
          iVar1 = param_2 + uVar3;
        } while (uVar3 < param_4);
      }
      uVar2 = uVar2 + 1;
    } while (uVar2 < param_5);
  }
  return;
}


RT_GUEST_FN(0x002716e8, guest::textGridFillRectBlank)

}  // namespace guest
