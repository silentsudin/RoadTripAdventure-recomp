// guest:0x00240bbc transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t switchD_002407e4_proximityEventNoOpCase(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  return 0;
}


RT_GUEST_FN(0x00240bbc, guest::switchD_002407e4_proximityEventNoOpCase)

}  // namespace guest
