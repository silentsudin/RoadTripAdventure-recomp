// guest:0x00286418 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceTtyRead(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x20, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x50) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x54) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::setS32(2, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(4, (uint32_t)param_1);  // walk: = param_1
  }
  char cVar1;
  int iVar2;
  int iVar3;
  
  iVar3 = 0;
  iVar2 = iVar3;
  if (0 < param_2) {
    do {
      iVar3 = iVar2 + 1;
      do {
      } while ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G(0x1776aa8)) + 4))) == 0);
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + iVar2))) = (*(int8_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G(0x1776aa8)) + 8)))));
      RT_CALLRA(0x286490, guest::ttyRingPop((*(int32_t*)rtguest::G(0x1776aa8))));
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + iVar2)));
    } while (((cVar1 != '\n') && (cVar1 != '\r')) && (iVar2 = iVar3, iVar3 < param_2));
  }
  return iVar3;
}


RT_GUEST_FN(0x00286418, guest::sceTtyRead)

}  // namespace guest
