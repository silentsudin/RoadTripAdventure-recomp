// guest:0x002767c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __kernel_cosf(uint32_t param_1, uint32_t param_2, ...) {
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  
  fVar1 = ABS(param_1);
  if ((0x31ffffff < (uint32_t)fVar1) || (fVar3 = 1.0, (int)param_1 != 0)) {
    fVar4 = param_1 * param_1;
    fVar3 = fVar4 * (fVar4 * (fVar4 * (fVar4 * (fVar4 * (fVar4 * -1.1359648e-11 + 2.0875723e-09) +
                                               -2.7557314e-07) + 2.4801588e-05) + -0.0013888889) +
                    0.041666668);
    if ((uint32_t)fVar1 < 0x3e99999a) {
      fVar3 = 1.0 - (fVar4 * 0.5 - (fVar4 * fVar3 - param_1 * param_2));
    }
    else {
      fVar2 = 0.28125;
      if ((uint32_t)fVar1 < 0x3f480001) {
        fVar2 = (float)((int)fVar1 - 0x1000000);
      }
      fVar3 = (1.0 - fVar2) - ((fVar4 * 0.5 - fVar2) - (fVar4 * fVar3 - param_1 * param_2));
    }
  }
}


RT_GUEST_FN(0x002767c8, guest::__kernel_cosf)

}  // namespace guest
