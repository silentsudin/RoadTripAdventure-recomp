// guest:0x0021d3e0 translated from Ghidra pseudocode and VU0 disassembly.
#include "../guest_vu0.h"

namespace guest {
uint32_t vu0LoadStateAndConstants(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  using namespace rtguest::vu0;
  S.vf[4] = lqc2(param_1); S.vf[5] = lqc2(param_1 + 16);
  S.vf[6] = lqc2(param_1 + 32); S.vf[7] = lqc2(param_1 + 48);
  viadd(1, 0, 0);
  S.vf[21] = vlqi(21, 1); S.vf[22] = vlqi(22, 1); S.vf[23] = vlqi(23, 1);
  S.vf[20] = vlqi(20, 1); S.vf[17] = vlqi(17, 1); S.vf[18] = vlqi(18, 1); S.vf[19] = vlqi(19, 1);
  S.vf[29] = {{4096.f, 4096.f, 4096.f, 4096.f}};
  S.vf[14] = {{128.f, 128.f, 128.f, 128.f}};
  ctc2(1, 0x8000); ctc2(2, 0xff0);
  return 0xff0;
}
RT_GUEST_FN(0x0021d3e0, guest::vu0LoadStateAndConstants)
}  // namespace guest
