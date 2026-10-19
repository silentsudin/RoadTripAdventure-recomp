// guest:0x00271568 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t textGridSetCursor(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  uint uVar1;
  uint uVar2;
  
  uVar2 = (*(uint32_t*)rtguest::G((uint32_t)param_1)) - 1;
  if (param_2 < (*(uint32_t*)rtguest::G((uint32_t)param_1))) {
    uVar2 = param_2;
  }
  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint32_t))) - 1;
  if (param_3 < (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint32_t)))) {
    uVar1 = param_3;
  }
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(uint32_t))) = uVar2;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(5) * sizeof(uint32_t))) = uVar1;
  return 0;
}


RT_GUEST_FN(0x00271568, guest::textGridSetCursor)

}  // namespace guest
