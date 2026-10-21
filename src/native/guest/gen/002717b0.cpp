// guest:0x002717b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

void textGridMoveRect(uint64_t param_1, uint64_t param_2, uint64_t param_3, uint64_t param_4, uint64_t param_5, uint32_t param_6, uint32_t param_7) {
  rtguest::FrameGuard rt_fg(176);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0x90, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x80, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x70, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x50, 8);
    *(uint32_t*)rtguest::G(rt_sv+0xa0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xa4) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x0) = (uint32_t)(int32_t)param_2;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = (uint32_t)(int32_t)param_4;
    rtguest::setS32(4, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(6, (uint32_t)param_6);  // walk: = param_6
    rtguest::setS32(7, (uint32_t)param_5);  // walk: = param_5
    rtguest::setS32(8, (uint32_t)param_3);  // walk: = param_3
  }
  undefined8 uVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  
  if ((param_5 <= param_3) && ((param_3 != param_5 || (param_4 <= param_2)))) {
    do {
      param_6 = param_6 - 1;
      uVar1 = RT_CALLRA(0x271848, guest::textBufferGetPixel16(param_1,(int)param_4 + param_6,(int)param_5 + (param_7 - 1)));
      RT_CALLRA(0x271860, guest::textBufferSetPixel16(param_1,(int)param_2 + param_6,(int)param_3 + (param_7 - 1),uVar1));
    } while( true );
  }
  uVar4 = 0;
  if (param_7 != 0) {
    do {
      uVar3 = 0;
      if (param_6 != 0) {
        do {
          iVar2 = (int)param_2 + uVar3;
          uVar1 = RT_CALLRA(0x2718c0, guest::textBufferGetPixel16(param_1,(int)param_4 + uVar3,(int)param_5 + uVar4));
          uVar3 = uVar3 + 1;
          RT_CALLRA(0x2718d8, guest::textBufferSetPixel16(param_1,iVar2,(int)param_3 + uVar4,uVar1));
        } while (uVar3 < param_6);
      }
      uVar4 = uVar4 + 1;
    } while (uVar4 < param_7);
  }
  return;
}


RT_GUEST_FN(0x002717b0, guest::textGridMoveRect)

}  // namespace guest
