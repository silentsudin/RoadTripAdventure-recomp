// guest:0x0021d2d8 translated from Ghidra pseudocode and VU0 disassembly.
#include "../guest_vu0.h"

namespace guest {
uint32_t TransformVecByF15Matrix(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(0);
  using namespace rtguest::vu0;
  S.vf[8] = lqc2(param_3);
  S.vf[4] = lqc2(param_2);
  S.vf[5] = lqc2(param_2 + 16);
  S.vf[6] = lqc2(param_2 + 32);
  vitof15(0b1111, 8, 8);
  S.vf[7] = lqc2(param_2 + 48);
  vmulabc(0b1111, 0, 4, 8);
  vmaddabc(0b1111, 1, 5, 8);
  vmaddbc(0b1111, 2, 9, 6, 8);
  vftoi15(0b1111, 9, 9);
  sqc2(param_1, S.vf[9]);
  return 0;
}
RT_GUEST_FN(0x0021d2d8, guest::TransformVecByF15Matrix)
}  // namespace guest
