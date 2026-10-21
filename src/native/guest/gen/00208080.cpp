// guest:0x00208080 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t ApplyMatrixRotation(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x40, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x48) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4c) = 0x00000000u;
    rtguest::rtSpillF(rt_fg, 4, rt_sv, 0x50);
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
  }
uint32_t auStack_60 = rtguest::allocFrame(64);
  
  RT_CALLRA(0x208098, guest::vu0UnitMatrix(auStack_60));
  RT_CALLRA(0x2080a8, guest::vu0RotMatrixY(param_1,auStack_60,auStack_60));
  RT_CALLRA(0x2080b8, guest::vu0MulMatrix(param_2,param_2,auStack_60));
  return 0;
}


RT_GUEST_FN(0x00208080, guest::ApplyMatrixRotation)

}  // namespace guest
