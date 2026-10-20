// guest:0x0023d040 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t switchD_0023cfc8_checkUnlockGate1c(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  return ((*(uint32_t*)rtguest::G(0x177fdd0)) & 0x20000) == 0;
}


RT_GUEST_FN(0x0023d040, guest::switchD_0023cfc8_checkUnlockGate1c)

}  // namespace guest
