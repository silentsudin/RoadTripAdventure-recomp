// guest:0x00272940 translated from Ghidra pseudocode and VU0 macro disassembly
#include "../guest_vu0.h"

namespace guest {
using namespace rtguest::vu0;

void vu0AddVector(uint32_t a,uint32_t b,uint32_t c) {
  rtguest::FrameGuard rt_fg(0);
S.vf[4]=lqc2(b); S.vf[5]=lqc2(c); vadd(15,6,4,5); sqc2(a,S.vf[6]);
}

RT_GUEST_FN(0x00272940, guest::vu0AddVector)

}  // namespace guest
