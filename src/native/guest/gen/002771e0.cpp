// guest:0x002771e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __kernel_sinf(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  float fVar1;
  float fVar2;
  float fVar3;
  
  if ((0x31ffffff < (uint32_t)ABS(param_1)) || ((int)param_1 != 0)) {
    fVar2 = param_1 * param_1;
    fVar3 = fVar2 * param_1;
    fVar1 = fVar2 * (fVar2 * (fVar2 * (fVar2 * 1.589691e-10 + -2.505076e-08) + 2.7557314e-06) +
                    -0.0001984127) + 0.008333334;
    if (param_3 == 0) {
      param_1 = param_1 + fVar3 * (fVar2 * fVar1 + -0.16666667);
    }
    else {
      param_1 = param_1 - ((fVar2 * (param_2 * 0.5 - fVar3 * fVar1) - param_2) - fVar3 * -0.16666667
                          );
    }
  }
}


RT_GUEST_FN(0x002771e0, guest::__kernel_sinf)

}  // namespace guest
