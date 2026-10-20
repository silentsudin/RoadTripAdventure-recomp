// guest:0x0020e4e8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ToggleInputFlagWrapper(uint64_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  debugScreenRead(param_1 ^ 1);
  return 0;
}


RT_GUEST_FN(0x0020e4e8, guest::ToggleInputFlagWrapper)

}  // namespace guest
