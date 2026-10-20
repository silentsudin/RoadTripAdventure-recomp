// guest:0x00226298 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t TransformMatrixAndSubmit(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
uint32_t auStack_50 = rtguest::allocFrame(64);
  
  vu0MulMatrix(auStack_50,0x177f5e0,param_1);
  vu0StoreMatrixQuadsAt4(auStack_50);
  return 0;
}


RT_GUEST_FN(0x00226298, guest::TransformMatrixAndSubmit)

}  // namespace guest
