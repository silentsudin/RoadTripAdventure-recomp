// guest:0x00273288 translated from Ghidra pseudocode and VU0 macro disassembly
#include "../guest_vu0.h"

namespace guest {
using namespace rtguest::vu0;

void vu0InterVectorXyz(uint32_t a,uint32_t b,uint32_t c,float d) {
  rtguest::FrameGuard rt_fg(0);
S.vf[4]=lqc2(b); S.vf[5]=lqc2(c); S.vf[6]={{d,0.f,0.f,0.f}}; vmove(1,9,4); vaddbc(8,3,7,0,0); vsub(8,8,7,6); vmulabc(14,0,4,6); vmaddbc(14,0,9,5,8); sqc2(a,S.vf[9]);
}

RT_GUEST_FN(0x00273288, guest::vu0InterVectorXyz)

}  // namespace guest
