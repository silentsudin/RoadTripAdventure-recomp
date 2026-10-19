// guest:0x00243a78 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ComputeScaledPartValue(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar1 = ComputeSlotScore(param_3);
  iVar2 = ComputeSlotScore(param_3 ^ 1);
  iVar3 = DispatchByIndex15(param_1,param_2);
  if (param_4 == 0) {
    iVar1 = (iVar3 * (2000 - (iVar1 - iVar2))) / 2000;
  }
  else {
    iVar1 = (iVar3 * ((iVar1 - iVar2) + 3000)) / 4000;
  }
  return iVar1;
}


RT_GUEST_FN(0x00243a78, guest::ComputeScaledPartValue)

}  // namespace guest
