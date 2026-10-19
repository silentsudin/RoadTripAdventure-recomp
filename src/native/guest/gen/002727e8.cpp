// guest:0x002727e8 translated from Ghidra pseudocode and VU0 macro disassembly
#include "../guest_vu0.h"

namespace guest {
using namespace rtguest::vu0;

void vecNormalizeVu0(uint32_t a,uint32_t b) {
S.vf[4]=lqc2(b); vmul(14,5,4,4); vaddbc(8,1,5,5,5); vaddbc(8,2,5,5,5); vsqrt(0,5); vwaitq(); vaddq(8,5,0); vdiv(3,0,0,5); vsub(15,6,0,0); vwaitq(); vmulq(14,6,4); sqc2(a,S.vf[6]);
}

RT_GUEST_FN(0x002727e8, guest::vecNormalizeVu0)

}  // namespace guest
