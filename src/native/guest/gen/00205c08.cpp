// guest:0x00205c08 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiMenuRun(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  uiMenuWindow(param_1,param_2,0);
  return 0;
}


RT_GUEST_FN(0x00205c08, guest::uiMenuRun)

}  // namespace guest
