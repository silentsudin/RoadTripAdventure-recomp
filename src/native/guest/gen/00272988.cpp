// guest:0x00272988 translated from Ghidra pseudocode and VU0 macro disassembly
#include "../guest_vu0.h"

namespace guest {
using namespace rtguest::vu0;

void vu0ScaleVector(float a,uint32_t b,uint32_t c) {
S.vf[4]=lqc2(c); S.vf[5]={{a,0.f,0.f,0.f}}; vmulbc(15,0,6,4,5); sqc2(b,S.vf[6]);
}

RT_GUEST_FN(0x00272988, guest::vu0ScaleVector)

}  // namespace guest
