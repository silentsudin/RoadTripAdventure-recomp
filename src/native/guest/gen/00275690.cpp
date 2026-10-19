// guest:0x00275690 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __ieee754_sqrtf(uint32_t param_1, ...) {
  int iVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  
  if (0x7fffff < (uint32_t)ABS(param_1)) {
    if (-1 < (int)param_1) {
      uVar5 = ((int)param_1 >> 0x17) - 0x7f;
      iVar6 = 0;
      uVar2 = 0x1000000;
      iVar3 = (((uint32_t)param_1 & 0x7fffff | 0x800000) << (uVar5 & 1)) << 1;
      uVar4 = 0;
      do {
        iVar1 = iVar6 + uVar2;
        if (iVar1 <= iVar3) {
          iVar3 = iVar3 - iVar1;
          iVar6 = iVar1 + uVar2;
          uVar4 = uVar4 + uVar2;
        }
        uVar2 = uVar2 >> 1;
        iVar3 = iVar3 << 1;
      } while (uVar2 != 0);
      if (iVar3 != 0) {
        uVar4 = uVar4 + (uVar4 & 1);
      }
      return (float)(((int)uVar4 >> 1) + 0x3f000000 + ((int)uVar5 >> 1) * 0x800000);
    }
    param_1 = (param_1 - param_1) / (param_1 - param_1);
  }
  return param_1;
}


RT_GUEST_FN(0x00275690, guest::__ieee754_sqrtf)

}  // namespace guest
