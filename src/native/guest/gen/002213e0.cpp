// guest:0x002213e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t renderInitNoOp(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  return 0;
}


RT_GUEST_FN(0x002213e0, guest::renderInitNoOp)

}  // namespace guest
