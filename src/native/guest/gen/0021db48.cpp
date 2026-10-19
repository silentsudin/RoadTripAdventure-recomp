// guest:0x0021db48 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gifAdPair(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  undefined8 uVar1;
  
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint64_t))) = param_2;
  uVar1 = (*(uint64_t*)rtguest::G(0x29cbc0));
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint64_t))) = param_3;
  (*(uint64_t*)rtguest::G((uint32_t)param_1)) = uVar1;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint64_t))) = (*(uint64_t*)rtguest::G(0x29cbc8));
  return 2;
}


RT_GUEST_FN(0x0021db48, guest::gifAdPair)

}  // namespace guest
