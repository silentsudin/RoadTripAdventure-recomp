// guest:0x002406c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t switchD_00240314_cameraTransitionUfoDetach(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2ec8e0u)));
  gs_tasks_kill();
  return 0;
}


RT_GUEST_FN(0x002406c8, guest::switchD_00240314_cameraTransitionUfoDetach)

}  // namespace guest
