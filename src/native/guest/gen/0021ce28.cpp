// guest:0x0021ce28 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetAICarController(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  if (param_1 == (uint32_t)0x0) {
    param_1 = 0x21d0b8u;
  }
  (*(uint32_t*)rtguest::G(0x3356dc)) = param_1;
  return 0;
}


RT_GUEST_FN(0x0021ce28, guest::SetAICarController)

}  // namespace guest
