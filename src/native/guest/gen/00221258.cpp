// guest:0x00221258 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t tileSlotLoaded(uint32_t param_1, uint32_t param_2, ...) {
  uint uVar1;
  
  uVar1 = 0;
  if ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((param_2 & 3) * 4 + param_1 + 0x60))) == param_2) {
    uVar1 = (uint32_t)~(int)(char)(*(uint8_t*)rtguest::G((uint32_t)0x3335b8 + (uint32_t)(param_2 & 3) * 1)) >> 0x1f;
  }
}


RT_GUEST_FN(0x00221258, guest::tileSlotLoaded)

}  // namespace guest
