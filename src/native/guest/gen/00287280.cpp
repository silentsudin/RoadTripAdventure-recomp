// guest:0x00287280 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifAddCmdHandler(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
uint32_t puVar1;
  int iVar2;
  
  iVar2 = (*(uint32_t*)rtguest::G(0x1776eac));
  if ((int32_t)param_1 < 0) {
    iVar2 = (*(int32_t*)rtguest::G(0x1776ea4));
  }
  puVar1 = (uint32_t)((int)param_1 * 8 + iVar2);
  (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(1) * sizeof(uint32_t))) = param_3;
  (*(uint32_t*)rtguest::G((uint32_t)puVar1)) = param_2;
  return 0;
}


RT_GUEST_FN(0x00287280, guest::sceSifAddCmdHandler)

}  // namespace guest
