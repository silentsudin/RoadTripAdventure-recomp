// guest:0x00282838 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __sfmoreglue(uint32_t param_1, uint32_t param_2, ...) {
  long lVar1;
uint32_t puVar2;
  
  lVar1 = _malloc_r(param_1,param_2 * 0x58 + 0xc);
  puVar2 = (uint32_t)lVar1;
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(1) * sizeof(uint32_t))) = param_2;
    (*(uint32_t*)rtguest::G((uint32_t)puVar2)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(2) * sizeof(uint32_t))) = puVar2 + 3;
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(puVar2 + 3)), 0, (long)(param_2 * 0x58)));
  }
  return lVar1;
}


RT_GUEST_FN(0x00282838, guest::__sfmoreglue)

}  // namespace guest
