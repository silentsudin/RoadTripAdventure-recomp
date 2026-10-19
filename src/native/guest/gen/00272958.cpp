// guest:0x00272958 translated from Ghidra pseudocode and VU0 macro disassembly
#include "../guest_vu0.h"

namespace guest {
using namespace rtguest::vu0;

void vu0SubVector(uint32_t a,uint32_t b,uint32_t c) {
S.vf[4]=lqc2(b); S.vf[5]=lqc2(c); vsub(15,6,4,5); sqc2(a,S.vf[6]);
}

RT_GUEST_FN(0x00272958, guest::vu0SubVector)

}  // namespace guest
