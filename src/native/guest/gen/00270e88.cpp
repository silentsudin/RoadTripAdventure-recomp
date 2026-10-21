// guest:0x00270e88 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

void textGridEmitRows(uint32_t param_1, uint64_t param_2) {
  rtguest::FrameGuard rt_fg(128);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x70) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x74) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x50, 8);
    rtguest::gS[0] = 0x0000000000000000ull;  // walk const
    rtguest::setS32(6, (uint32_t)param_2);  // walk: = param_2
  }
  uint uVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  
  uVar3 = 0;
  uVar1 = (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t)));
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_1));
  iVar4 = (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int32_t)));
  if (uVar1 != 0) {
    do {
      textEmitGlyphsToGs(param_2,param_1 + 6,0,uVar3,iVar4,iVar2);
      uVar3 = uVar3 + 1;
      iVar4 = iVar4 + iVar2 * 2;
    } while (uVar3 < uVar1);
  }
  return;
}


RT_GUEST_FN(0x00270e88, guest::textGridEmitRows)

}  // namespace guest
