// guest:0x002357f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t switchD_002357e8_dispatchTypeSetupCase12(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  UpdateTimedPhaseTask(param_1,param_2,0,0,0);
  return 0;
}


RT_GUEST_FN(0x002357f0, guest::switchD_002357e8_dispatchTypeSetupCase12)

}  // namespace guest
