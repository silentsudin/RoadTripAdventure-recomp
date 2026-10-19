// guest:0x002728d8 translated from Ghidra pseudocode and VU0 macro disassembly
#include "../guest_vu0.h"

namespace guest {
using namespace rtguest::vu0;

void vecDivByScalarVu0(float a,uint32_t b,uint32_t c) {
S.vf[4]=lqc2(c); S.vf[5]={{a,0.f,0.f,0.f}}; vdiv(3,0,0,5); vwaitq(); vmulq(15,4,4); sqc2(b,S.vf[4]);
}

RT_GUEST_FN(0x002728d8, guest::vecDivByScalarVu0)

}  // namespace guest
