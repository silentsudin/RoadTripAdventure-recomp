// guest:0x0023d5b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CallFindFlaggedSlot(uint32_t q0, ...) {
  FindFlaggedSlotIndex();
  return 0;
}


RT_GUEST_FN(0x0023d5b0, guest::CallFindFlaggedSlot)

}  // namespace guest
