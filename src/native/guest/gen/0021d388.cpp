// guest:0x0021d388 translated from Ghidra pseudocode and VU0 disassembly.
#include "../guest_vu0.h"

namespace guest {
uint32_t vu0StoreMatrixQuads(uint32_t param_1, ...) {
  using namespace rtguest::vu0;
  S.vf[1] = lqc2(param_1);
  S.vf[2] = lqc2(param_1 + 16);
  S.vf[3] = lqc2(param_1 + 32);
  S.vf[4] = lqc2(param_1 + 48);
  viadd(1, 0, 0);
  vsqi(0b1111, 1, 1); vsqi(0b1111, 2, 1);
  vsqi(0b1111, 3, 1); vsqi(0b1111, 4, 1);
  return 0;
}
RT_GUEST_FN(0x0021d388, guest::vu0StoreMatrixQuads)
}  // namespace guest
