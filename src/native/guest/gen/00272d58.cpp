// guest:0x00272d58 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t vu0CameraMatrix(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(176);
uint32_t auStack_b0 = rtguest::allocFrame(16);
uint32_t auStack_a0 = rtguest::allocFrame(16);
uint32_t auStack_90 = rtguest::allocFrame(32);
uint32_t auStack_70 = rtguest::allocFrame(16);
  
  vu0UnitMatrix(auStack_b0);
  vecCrossProductVu0(auStack_70,param_4,param_3);
  vecNormalizeVu0(auStack_b0,auStack_70);
  vecNormalizeVu0(auStack_90,param_3);
  vecCrossProductVu0(auStack_a0,auStack_90,auStack_b0);
  vu0TransMatrix(auStack_b0,auStack_b0,param_2);
  mat4InvertAffine(param_1,auStack_b0);
  return 0;
}


RT_GUEST_FN(0x00272d58, guest::vu0CameraMatrix)

}  // namespace guest
