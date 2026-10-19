// guest:0x00272700 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gifPkWriteQword(uint32_t param_1, uint32_t param_2, ...) {
uint32_t puVar1;
  
  puVar1 = (uint32_t)(*(int32_t*)rtguest::G((uint32_t)param_1));
  (*(uint32_t*)rtguest::G((uint32_t)puVar1)) = (int)param_2;
  (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(1) * sizeof(uint32_t))) = (int)((uint32_t)param_2 >> 0x20);
  (*(int32_t*)rtguest::G((uint32_t)param_1)) = (int)(puVar1 + 2);
  return 0;
}


RT_GUEST_FN(0x00272700, guest::gifPkWriteQword)

}  // namespace guest
