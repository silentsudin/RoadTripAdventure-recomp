// guest:0x00277af8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t scalbnf(uint32_t param_1, uint32_t param_2, ...) {
  int iVar1;
  float fVar2;
  
  iVar1 = ((uint32_t)ABS(param_1) >> 0x17) + (int)param_2;
  if (0x7fffff < (uint32_t)ABS(param_1)) {
    if (iVar1 < 0x100) {
      if (0 < iVar1) {
        return (float)((uint32_t)param_1 & 0x807fffff | iVar1 * 0x800000);
      }
      if (param_2 < 0xc351) {
        fVar2 = 1e-30;
      }
      else {
        fVar2 = 1e+30;
      }
    }
    else {
      fVar2 = 1e+30;
    }
    param_1 = (float)copysignf(fVar2,param_1);
    param_1 = param_1 * fVar2;
  }
}


RT_GUEST_FN(0x00277af8, guest::scalbnf)

}  // namespace guest
