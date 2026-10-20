// guest:0x002767c8 translated from Ghidra pseudocode and ELF disassembly
// k_cosf: guard 2nd test is cvt.w.s(x)!=0 (0x2767dc); iz at 0x2768f0 is addu
// 0xff000000 on |x| bits (= bits - 0x1000000, then used as float).
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __kernel_cosf(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  float x = rtguest::fbits(param_1);
  float y = rtguest::fbits(param_2);
  uint32_t fVar1 = param_1 & 0x7fffffffu;  // |x| as bit pattern (and 0x7fffffff)
  float fVar2;
  float fVar3;
  float fVar4;

  if (0x31ffffffu < fVar1 || (fVar3 = 1.0f, (int32_t)x != 0)) {
    fVar4 = x * x;
    fVar3 = fVar4 * (fVar4 * (fVar4 * (fVar4 * (fVar4 * (fVar4 * -1.1359648e-11f + 2.0875723e-09f) +
                                               -2.7557314e-07f) + 2.4801588e-05f) + -0.0013888889f) +
                    0.041666668f);
    if (fVar1 < 0x3e99999au) {
      fVar3 = 1.0f - (fVar4 * 0.5f - (fVar4 * fVar3 - x * y));
    }
    else {
      fVar2 = 0.28125f;
      if (fVar1 < 0x3f480001u) {
        fVar2 = rtguest::fbits(fVar1 - 0x1000000u);  // addu on bits (0x2768f0)
      }
      fVar3 = (1.0f - fVar2) - ((fVar4 * 0.5f - fVar2) - (fVar4 * fVar3 - x * y));
    }
  }
  return rtguest::f2bits(fVar3);
}


RT_GUEST_FN(0x002767c8, guest::__kernel_cosf)

}  // namespace guest
