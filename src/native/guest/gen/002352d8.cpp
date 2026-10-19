// guest:0x002352d8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ResetMode1B(uint32_t param_1, ...) {
uint32_t pvVar1;
  
  pvVar1 = (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(param_1)), 0, 0x8cb0));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8004))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8008))) = 0x1b;
}


RT_GUEST_FN(0x002352d8, guest::ResetMode1B)

}  // namespace guest
