// guest:0x002727a0 translated from Ghidra pseudocode and VU0 macro disassembly
#include "../guest_vu0.h"

namespace guest {
using namespace rtguest::vu0;

void vecCrossProductVu0(uint32_t a,uint32_t b,uint32_t c) {
  rtguest::FrameGuard rt_fg;
S.vf[4]=lqc2(b); S.vf[5]=lqc2(c); vopmula(14,4,5); vopmsub(14,6,5,4); vsub(1,6,6,6); sqc2(a,S.vf[6]);
}

RT_GUEST_FN(0x002727a0, guest::vecCrossProductVu0)

}  // namespace guest
