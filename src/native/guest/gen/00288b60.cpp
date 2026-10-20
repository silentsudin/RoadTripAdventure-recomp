// guest:0x00288b60 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _sceFsSigSema(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  SignalSema((*(uint32_t*)rtguest::G(0x2e998c)));
  return 0;
}


RT_GUEST_FN(0x00288b60, guest::_sceFsSigSema)

}  // namespace guest
