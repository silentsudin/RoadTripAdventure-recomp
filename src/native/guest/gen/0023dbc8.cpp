// guest:0x0023dbc8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CheckSlotPresence(uint32_t q0, ...) {
  undefined4 uVar1;
  
  uVar1 = 1;
  if (((*(int8_t*)rtguest::G(0x177fdd4)) != '\0') && (uVar1 = 2, (*(int8_t*)rtguest::G(0x177fdd6)) != '\0')) {
    uVar1 = 0;
  }
}


RT_GUEST_FN(0x0023dbc8, guest::CheckSlotPresence)

}  // namespace guest
