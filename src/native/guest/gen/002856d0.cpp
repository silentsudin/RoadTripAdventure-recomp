// guest:0x002856d0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sysKill(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  if (param_1 == 1) {
    _Exit(param_2);
  }
  return 0;
}


RT_GUEST_FN(0x002856d0, guest::sysKill)

}  // namespace guest
