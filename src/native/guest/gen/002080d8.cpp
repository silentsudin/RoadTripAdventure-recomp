// guest:0x002080d8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SelectMeshIntersectionRoutine(uint64_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  if (param_1 == 0) {
    (*(uint32_t*)rtguest::G(0x335710)) = 0x207280u;
  }
  else {
    (*(uint32_t*)rtguest::G(0x335710)) = 0x2084d0u;
  }
  return 0;
}


RT_GUEST_FN(0x002080d8, guest::SelectMeshIntersectionRoutine)

}  // namespace guest
