// guest:0x00272d58 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t vu0CameraMatrix(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(176);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x90, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x80, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x70, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x50, 8);
    *(uint32_t*)rtguest::G(rt_sv+0xa0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xa4) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_4);  // walk: = param_4
    rtguest::setS32(2, (uint32_t)param_3);  // walk: = param_3
    rtguest::setS32(3, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(4, (uint32_t)param_1);  // walk: = param_1
  }
uint32_t auStack_b0 = rtguest::allocFrame(16);
uint32_t auStack_a0 = rtguest::allocFrame(16);
uint32_t auStack_90 = rtguest::allocFrame(32);
uint32_t auStack_70 = rtguest::allocFrame(16);
  
  RT_CALLRA(0x272d84, guest::vu0UnitMatrix(auStack_b0));
  RT_CALLRA(0x272d98, guest::vecCrossProductVu0(auStack_70,param_4,param_3));
  RT_CALLRA(0x272da4, guest::vecNormalizeVu0(auStack_b0,auStack_70));
  RT_CALLRA(0x272db4, guest::vecNormalizeVu0(auStack_90,param_3));
  RT_CALLRA(0x272dc4, guest::vecCrossProductVu0(auStack_a0,auStack_90,auStack_b0));
  RT_CALLRA(0x272dd4, guest::vu0TransMatrix(auStack_b0,auStack_b0,param_2));
  RT_CALLRA(0x272de0, guest::mat4InvertAffine(param_1,auStack_b0));
  return 0;
}


RT_GUEST_FN(0x00272d58, guest::vu0CameraMatrix)

}  // namespace guest
