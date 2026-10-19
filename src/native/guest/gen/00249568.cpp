// guest:0x00249568 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CalculateBoostGaugeValue(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  int iVar1;
  
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x23c)));
  if (((param_2 != 0) || (param_3 != 0)) && (0x40000 < iVar1)) {
    iVar1 = 0x40000;
  }
  if (param_2 != 0) {
    iVar1 = (iVar1 * (int)param_2 + 0x3ffff) / 0x40000;
  }
  return iVar1;
}


RT_GUEST_FN(0x00249568, guest::CalculateBoostGaugeValue)

}  // namespace guest
