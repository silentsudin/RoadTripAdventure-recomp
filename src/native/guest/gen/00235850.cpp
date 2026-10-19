// guest:0x00235850 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t switchD_002357e8_dispatchTypeSetupCase1e(uint32_t param_1, uint32_t param_2, ...) {
  UpdateTimedPhaseTask(param_1,param_2,3,0,0);
  return 0;
}


RT_GUEST_FN(0x00235850, guest::switchD_002357e8_dispatchTypeSetupCase1e)

}  // namespace guest
