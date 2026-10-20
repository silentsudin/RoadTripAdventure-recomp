// guest:0x002833f8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t setlocale(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  _setlocale_r((*(uint32_t*)rtguest::G(0x002e8cc0)),param_1,param_2);
  return 0;
}


RT_GUEST_FN(0x002833f8, guest::setlocale)

}  // namespace guest
