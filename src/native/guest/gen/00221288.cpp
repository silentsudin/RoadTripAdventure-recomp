// guest:0x00221288 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t tileSlotOrigin(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  char cVar1;
  
  cVar1 = (*(uint8_t*)rtguest::G((uint32_t)0x3335b8 + (uint32_t)(param_3) * 1));
  if (-1 < cVar1) {
    vu0AddVector(param_1,param_2,0x29d080 + cVar1 * 0x10);
  }
  return -1 < cVar1;
}


RT_GUEST_FN(0x00221288, guest::tileSlotOrigin)

}  // namespace guest
