// guest:0x002356d0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t InitObjectType24(uint32_t param_1, ...) {
uint32_t pvVar1;
  
  pvVar1 = (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(param_1)), 0, 0x8cb0));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8004))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8008))) = 0x24;
  return pvVar1;
}


RT_GUEST_FN(0x002356d0, guest::InitObjectType24)

}  // namespace guest
