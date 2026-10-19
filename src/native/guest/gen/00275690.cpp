// guest:0x00275690 translated from Ghidra pseudocode (= fdlibm e_sqrtf.c):
// pure integer digit-by-digit sqrt on the f-register bits of |x|; result
// re-encoded into float bits. Negative x -> div.s 0/0 quiet NaN.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __ieee754_sqrtf(uint32_t param_1, ...) {
  int iVar1;
  uint32_t uVar2;
  int iVar3;
  uint32_t uVar4;
  uint32_t uVar5;
  int iVar6;

  if (0x7fffffu < (param_1 & 0x7fffffffu)) {
    if (-1 < (int32_t)param_1) {  // sign bit of x
      uVar5 = ((int32_t)param_1 >> 0x17) - 0x7f;
      iVar6 = 0;
      uVar2 = 0x1000000;
      iVar3 = (((param_1 & 0x7fffffu | 0x800000u) << (uVar5 & 1)) << 1);
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
      // mtc1 of the re-encoded integer into f0
      return (uint32_t)(((int32_t)uVar4 >> 1) + 0x3f000000 + ((int32_t)uVar5 >> 1) * 0x800000);
    }
    param_1 = rtguest::f2bits(0.0f / 0.0f);  // div.s 0/0 quiet NaN
  }
  return param_1;
}


RT_GUEST_FN(0x00275690, guest::__ieee754_sqrtf)

}  // namespace guest
