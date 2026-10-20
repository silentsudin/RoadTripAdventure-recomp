// guest:0x0023d5c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CallCountFlaggedSlots(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  CountFlaggedSlots();
  return 0;
}


RT_GUEST_FN(0x0023d5c8, guest::CallCountFlaggedSlots)

}  // namespace guest
