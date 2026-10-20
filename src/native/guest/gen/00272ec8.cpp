// guest:0x00272ec8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t vu0LightColorMatrix(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, ...) {
  rtguest::FrameGuard rt_fg;
  vu0CopyVector();
  vu0CopyVector(param_1 + 0x10,param_3);
  vu0CopyVector(param_1 + 0x20,param_4);
  vu0CopyVector(param_1 + 0x30,param_5);
  return 0;
}


RT_GUEST_FN(0x00272ec8, guest::vu0LightColorMatrix)

}  // namespace guest
