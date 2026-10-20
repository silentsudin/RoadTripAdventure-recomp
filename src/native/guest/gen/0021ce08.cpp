// guest:0x0021ce08 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SelectScene3dHandler(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(0);
  if (param_1 == 0) {
    (*(uint32_t*)rtguest::G(0x3356d8)) = 0x21cf80u;
  }
  else {
    (*(uint32_t*)rtguest::G(0x3356d8)) = 0x21ce48u;
  }
  return 0;
}


RT_GUEST_FN(0x0021ce08, guest::SelectScene3dHandler)

}  // namespace guest
