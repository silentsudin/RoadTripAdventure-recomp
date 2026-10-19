// guest:0x00272a48 translated from Ghidra pseudocode and VU0 macro disassembly
#include "../guest_vu0.h"

namespace guest {
using namespace rtguest::vu0;

void vu0UnitMatrix(uint32_t a) {
vsub(15,4,0,0); vadd(1,4,4,0); vmr32(15,5,4); vmr32(15,6,5); vmr32(15,7,6); sqc2(a+48,S.vf[4]); sqc2(a+32,S.vf[5]); sqc2(a+16,S.vf[6]); sqc2(a,S.vf[7]);
}

RT_GUEST_FN(0x00272a48, guest::vu0UnitMatrix)

}  // namespace guest
