// guest:0x002771e0 translated from Ghidra pseudocode and ELF disassembly
// k_sinf: guard = (|x|bits > 0x31ffffff) || (cvt.w.s(x) != 0) (0x2771f4 cvt pair);
// params carry float bits, returned in f0.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __kernel_sinf(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  float x = rtguest::fbits(param_1);
  float y = rtguest::fbits(param_2);
  float fVar1;
  float fVar2;
  float fVar3;

  if ((param_1 & 0x7fffffffu) > 0x31ffffffu || (int32_t)x != 0) {
    fVar2 = x * x;
    fVar3 = fVar2 * x;
    fVar1 = fVar2 * (fVar2 * (fVar2 * (fVar2 * 1.589691e-10f + -2.505076e-08f) + 2.7557314e-06f) +
                    -0.0001984127f) + 0.008333334f;
    if (param_3 == 0) {
      x = x + fVar3 * (fVar2 * fVar1 + -0.16666667f);
    }
    else {
      x = x - ((fVar2 * (y * 0.5f - fVar3 * fVar1) - y) - fVar3 * -0.16666667f);
    }
    return rtguest::f2bits(x);
  }
  return param_1;
}


RT_GUEST_FN(0x002771e0, guest::__kernel_sinf)

}  // namespace guest
