// guest:0x00272728 translated from Ghidra pseudocode and VU0 macro disassembly
#include "../guest_vu0.h"

namespace guest {
using namespace rtguest::vu0;

void vu0ApplyMatrix(uint32_t a,uint32_t b,uint32_t c) {
S.vf[4]=lqc2(b); S.vf[5]=lqc2(b+16); S.vf[6]=lqc2(b+32); S.vf[7]=lqc2(b+48); S.vf[8]=lqc2(c); vmulabc(15,0,4,8); vmaddabc(15,1,5,8); vmaddabc(15,2,6,8); vmaddbc(15,3,9,7,8); sqc2(a,S.vf[9]);
}

RT_GUEST_FN(0x00272728, guest::vu0ApplyMatrix)

}  // namespace guest
