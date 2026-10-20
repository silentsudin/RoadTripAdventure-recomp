// guest:0x0027ee40 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _getpid_r(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
  return 1;
}


RT_GUEST_FN(0x0027ee40, guest::_getpid_r)

}  // namespace guest
