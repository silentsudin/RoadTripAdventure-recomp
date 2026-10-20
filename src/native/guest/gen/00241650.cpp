// guest:0x00241650 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t switchD_002414d4_dispatchCourseEffectKill(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
  gs_tasks_kill();
  return 0;
}


RT_GUEST_FN(0x00241650, guest::switchD_002414d4_dispatchCourseEffectKill)

}  // namespace guest
