// guest:0x00272918 translated from Ghidra pseudocode and VU0 macro disassembly
#include "../guest_vu0.h"

namespace guest {
using namespace rtguest::vu0;

void vu0InterVector(float a,uint32_t b,uint32_t c,uint32_t d) {
  rtguest::FrameGuard rt_fg;
S.vf[4]=lqc2(c); S.vf[5]=lqc2(d); S.vf[6]={{a,0.f,0.f,0.f}}; vaddbc(8,3,7,0,0); vsub(8,8,7,6); vmulabc(15,0,4,6); vmaddbc(15,0,9,5,8); sqc2(b,S.vf[9]);
}

RT_GUEST_FN(0x00272918, guest::vu0InterVector)

}  // namespace guest
