// guest:0x0024fad0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

void DrawOptionSlider(uint32_t param_1, uint64_t param_2, uint32_t param_3, uint64_t param_4) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(1, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(2, (uint32_t)param_4);  // walk: = param_4
  }
  RT_CALLRA(0x24faf4, guest::uiDrawBar3(param_1 + 0x14,param_2,param_3 + -0x14));
  if (param_4 != 0) {
    uiDrawArrow(param_1 + -0x14,param_2);
    return;
  }
  return;
}


RT_GUEST_FN(0x0024fad0, guest::DrawOptionSlider)

}  // namespace guest
