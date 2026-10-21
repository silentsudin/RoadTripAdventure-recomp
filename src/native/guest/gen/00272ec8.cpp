// guest:0x00272ec8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t vu0LightColorMatrix(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x40) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x44) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(2, (uint32_t)param_4);  // walk: = param_4
    rtguest::setS32(3, (uint32_t)param_5);  // walk: = param_5
  }
  vu0CopyVector();
  vu0CopyVector(param_1 + 0x10,param_3);
  vu0CopyVector(param_1 + 0x20,param_4);
  vu0CopyVector(param_1 + 0x30,param_5);
  return 0;
}


RT_GUEST_FN(0x00272ec8, guest::vu0LightColorMatrix)

}  // namespace guest
