// guest:0x002779e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t fabsf(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  return param_1 & 0x7fffffff;
}


RT_GUEST_FN(0x002779e0, guest::fabsf)

}  // namespace guest
