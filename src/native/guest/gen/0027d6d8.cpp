// guest:0x0027d6d8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __unpack_f(uint32_t param_1, uint32_t param_2, ...) {
  uint uVar1;
  uint uVar2;
  
  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)param_1));
  uVar2 = uVar1 >> 0x17 & 0xff;
  (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(uint32_t))) = uVar1 >> 0x1f;
  if (uVar2 == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_2)) = 2;
    return 0;
  }
  (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(uint32_t))) = uVar2 - 0x7f;
  (*(uint32_t*)rtguest::G((uint32_t)param_2)) = 3;
  (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(3) * sizeof(uint32_t))) = (uVar1 & 0x7fffff) << 7 | 0x40000000;
  return 0;
}


RT_GUEST_FN(0x0027d6d8, guest::__unpack_f)

}  // namespace guest
