// guest:0x00235808 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t switchD_002357e8_dispatchTypeSetupCase18(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  UpdateTimedPhaseTask(param_1,param_2,0,1,1);
  return 0;
}


RT_GUEST_FN(0x00235808, guest::switchD_002357e8_dispatchTypeSetupCase18)

}  // namespace guest
