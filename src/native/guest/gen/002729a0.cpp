// guest:0x002729a0 translated from Ghidra pseudocode and VU0 macro disassembly
#include "../guest_vu0.h"

namespace guest {
using namespace rtguest::vu0;

void vu0TransMatrix(uint32_t a,uint32_t b,uint32_t c) {
S.vf[4]=lqc2(c); S.vf[5]=lqc2(b+48); vadd(14,5,5,4); sqc2(a+48,S.vf[5]); std::memcpy(rtguest::G(a),rtguest::G(b),48);
}

RT_GUEST_FN(0x002729a0, guest::vu0TransMatrix)

}  // namespace guest
