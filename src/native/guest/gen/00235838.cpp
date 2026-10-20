// guest:0x00235838 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t switchD_002357e8_dispatchTypeSetupCase14(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  UpdateTimedPhaseTask(param_1,param_2,2,0,0);
  return 0;
}


RT_GUEST_FN(0x00235838, guest::switchD_002357e8_dispatchTypeSetupCase14)

}  // namespace guest
