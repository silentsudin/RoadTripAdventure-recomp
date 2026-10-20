// guest:0x00272d30 translated from Ghidra pseudocode and VU0 macro disassembly
#include "../guest_vu0.h"

namespace guest {
using namespace rtguest::vu0;

void vu0ClampVector(float a,float b,uint32_t c,uint32_t d) {
  rtguest::FrameGuard rt_fg(176);
S.vf[6]=lqc2(d); S.vf[4]={{a,0.f,0.f,0.f}}; S.vf[5]={{b,0.f,0.f,0.f}}; vmaxbc(15,0,6,6,4); vminibc(15,0,6,6,5); sqc2(c,S.vf[6]);
}

RT_GUEST_FN(0x00272d30, guest::vu0ClampVector)

}  // namespace guest
