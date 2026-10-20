// guest:0x00208428 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t PlayTaskSound(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  PlayUiSound((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) + 0x8000);
  return 0;
}


RT_GUEST_FN(0x00208428, guest::PlayTaskSound)

}  // namespace guest
