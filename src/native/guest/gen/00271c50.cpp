// guest:0x00271c50 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t xorHighBitPair(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  return (uint16_t)param_1 ^ 0x8080;
}


RT_GUEST_FN(0x00271c50, guest::xorHighBitPair)

}  // namespace guest
