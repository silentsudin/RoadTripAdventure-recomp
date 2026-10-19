// guest:0x00272a18 translated from Ghidra pseudocode and VU0 macro disassembly
#include "../guest_vu0.h"

namespace guest {
using namespace rtguest::vu0;

void vu0Ftoi0Vector(uint32_t a,uint32_t b) {
S.vf[4]=lqc2(b); vftoi0(15,5,4); sqc2(a,S.vf[5]);
}

RT_GUEST_FN(0x00272a18, guest::vu0Ftoi0Vector)

}  // namespace guest
