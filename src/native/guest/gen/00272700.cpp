// guest:0x00272700 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// pseudocode: void gifPkWriteQword(int*, undefined8)
void gifPkWriteQword(uint32_t param_1, uint64_t param_2, ...) {
uint32_t puVar1;
  
  puVar1 = (uint32_t)(*(int32_t*)rtguest::G((uint32_t)param_1));
  (*(uint32_t*)rtguest::G((uint32_t)puVar1)) = (int)param_2;
  (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(1) * sizeof(uint32_t))) = (int)(param_2 >> 0x20);
  // pseudocode: puVar1 + 2 on undefined4* = +8 bytes
  (*(int32_t*)rtguest::G((uint32_t)param_1)) = (int)(puVar1 + (uint32_t)(2) * sizeof(uint32_t));
}


RT_GUEST_FN(0x00272700, guest::gifPkWriteQword)

}  // namespace guest
