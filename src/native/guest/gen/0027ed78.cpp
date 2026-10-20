// guest:0x0027ed78 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t signal(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  _signal_r((*(uint32_t*)rtguest::G(0x002e8cc0)),param_1,param_2);
  return 0;
}


RT_GUEST_FN(0x0027ed78, guest::signal)

}  // namespace guest
