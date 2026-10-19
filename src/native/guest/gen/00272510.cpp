// guest:0x00272510 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gifPkAlign16(uint32_t param_1, ...) {
uint32_t piVar1;
uint32_t puVar2;
  
  piVar1 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t)));
  for (puVar2 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)param_1)); ((uint32_t)puVar2 & 0xc) != 0; puVar2 = puVar2 + 1) {
    (*(uint32_t*)rtguest::G((uint32_t)puVar2)) = 0;
  }
  if (piVar1 != (uint32_t)0x0) {
    (*(int32_t*)rtguest::G((uint32_t)piVar1)) = (*(int32_t*)rtguest::G((uint32_t)piVar1)) + ((int)puVar2 - (int)piVar1 >> 4) + -1;
  }
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)param_1)) = puVar2;
}


RT_GUEST_FN(0x00272510, guest::gifPkAlign16)

}  // namespace guest
