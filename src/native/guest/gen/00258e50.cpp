// guest:0x00258e50 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t PlayUiSound(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  SndRpcPlayOrStop(param_1 | 0x40400000);
  return 0;
}


RT_GUEST_FN(0x00258e50, guest::PlayUiSound)

}  // namespace guest
