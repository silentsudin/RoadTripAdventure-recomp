// guest:0x00272a08 translated from Ghidra pseudocode and VU0 macro disassembly
#include "../guest_vu0.h"

namespace guest {
using namespace rtguest::vu0;

void vu0Ftoi4Vector(uint32_t a,uint32_t b) {
  rtguest::FrameGuard rt_fg(0);
S.vf[4]=lqc2(b); vftoi4(15,5,4); sqc2(a,S.vf[5]);
}

RT_GUEST_FN(0x00272a08, guest::vu0Ftoi4Vector)

}  // namespace guest
