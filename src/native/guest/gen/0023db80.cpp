// guest:0x0023db80 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t MatchSlotPairState(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  undefined4 uVar1;
  
  if (((((*(int8_t*)rtguest::G(0x177fdd4)) != param_1) || (uVar1 = 1, (*(int8_t*)rtguest::G(0x177fdd5)) != param_2)) &&
      (uVar1 = 0, (*(int8_t*)rtguest::G(0x177fdd6)) == param_1)) && (uVar1 = 2, (*(int8_t*)rtguest::G(0x177fdd7)) != param_2)) {
    uVar1 = 0;
  }
  return uVar1;
}


RT_GUEST_FN(0x0023db80, guest::MatchSlotPairState)

}  // namespace guest
