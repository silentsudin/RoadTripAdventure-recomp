// guest:0x0021d450 translated from Ghidra pseudocode and VU0 disassembly.
#include "../guest_vu0.h"

namespace guest {
uint32_t vu0LoadVf15Quad(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  rtguest::vu0::S.vf[15] = rtguest::vu0::lqc2(param_1);
  return 0;
}
RT_GUEST_FN(0x0021d450, guest::vu0LoadVf15Quad)
}  // namespace guest
