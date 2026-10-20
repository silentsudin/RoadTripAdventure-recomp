// guest:0x0023cfd0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t switchD_0023cfc8_checkUnlockGate17(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(0);
  return ((*(uint32_t*)rtguest::G(0x177fdd0)) & 0x8000) == 0;
}


RT_GUEST_FN(0x0023cfd0, guest::switchD_0023cfc8_checkUnlockGate17)

}  // namespace guest
