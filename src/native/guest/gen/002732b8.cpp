// guest:0x002732b8 translated from Ghidra pseudocode and VU0 macro disassembly
#include "../guest_vu0.h"

namespace guest {
using namespace rtguest::vu0;

void sceVu0ScaleVectorXYZ(float a,uint32_t b,uint32_t c) {
S.vf[4]=lqc2(c); S.vf[5]={{a,0.f,0.f,0.f}}; vmulbc(14,0,4,4,5); sqc2(b,S.vf[4]);
}

RT_GUEST_FN(0x002732b8, guest::sceVu0ScaleVectorXYZ)

}  // namespace guest
