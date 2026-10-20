// guest:0x0021d3b8 translated from Ghidra pseudocode and VU0 disassembly.
#include "../guest_vu0.h"

namespace guest {
uint32_t vu0StoreMatrixQuadsAt4(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(0);
  using namespace rtguest::vu0;
  ctc2(1, 4);
  S.vf[1] = lqc2(param_1); S.vf[2] = lqc2(param_1 + 16); S.vf[3] = lqc2(param_1 + 32);
  vsqi(0b1111, 1, 1); vsqi(0b1111, 2, 1); vsqi(0b1111, 3, 1);
  return 4;
}
RT_GUEST_FN(0x0021d3b8, guest::vu0StoreMatrixQuadsAt4)
}  // namespace guest
