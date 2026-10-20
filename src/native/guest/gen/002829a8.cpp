// guest:0x002829a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _cleanup_r(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  _fwalk(param_1,0x2826c0);
  return 0;
}


RT_GUEST_FN(0x002829a8, guest::_cleanup_r)

}  // namespace guest
