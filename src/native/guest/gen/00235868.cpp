// guest:0x00235868 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t switchD_002357e8_dispatchTypeSetupCase16(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(48);
  RT_CALLRA(0x235868, guest::RunCountdownTimerTask());
  return 0;
}


RT_GUEST_FN(0x00235868, guest::switchD_002357e8_dispatchTypeSetupCase16)

}  // namespace guest
