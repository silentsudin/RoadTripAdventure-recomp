// guest:0x00208080 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ApplyMatrixRotation(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(96);
uint32_t auStack_60 = rtguest::allocFrame(64);
  
  vu0UnitMatrix(auStack_60);
  vu0RotMatrixY(param_1,auStack_60,auStack_60);
  vu0MulMatrix(param_2,param_2,auStack_60);
  return 0;
}


RT_GUEST_FN(0x00208080, guest::ApplyMatrixRotation)

}  // namespace guest
