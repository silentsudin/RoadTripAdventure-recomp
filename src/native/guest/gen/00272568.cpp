// guest:0x00272568 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gifPkAppendTagNreg1(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
uint32_t puVar1;
  int iVar2;
  
  iVar2 = gifPkAlign16();
  puVar1 = (uint32_t)(*(int32_t*)rtguest::G((uint32_t)param_1));
  (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int32_t))) = iVar2;
  (*(uint32_t*)rtguest::G((uint32_t)puVar1)) = param_4 | 0x10000000;
  (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(2) * sizeof(uint32_t))) = param_2;
  (*(int32_t*)rtguest::G((uint32_t)param_1)) = (int)(puVar1 + 4);
  (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(3) * sizeof(uint32_t))) = param_3;
  (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(1) * sizeof(uint32_t))) = 0;
  return 0;
}


RT_GUEST_FN(0x00272568, guest::gifPkAppendTagNreg1)

}  // namespace guest
