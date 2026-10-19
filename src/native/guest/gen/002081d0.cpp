// guest:0x002081d0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t FindSetBitRank(uint32_t param_1, uint32_t param_2, ...) {
  uint uVar1;
  int iVar2;
  
  uVar1 = 1 << (param_2 & 0x1f);
  if ((param_1 & uVar1) == 0) {
    return -1;
  }
  iVar2 = 0;
  for (param_1 = param_1 & uVar1 - 1; param_1 != 0; param_1 = param_1 >> 1) {
    iVar2 = iVar2 + (param_1 & 1);
  }
  return iVar2;
}


RT_GUEST_FN(0x002081d0, guest::FindSetBitRank)

}  // namespace guest
