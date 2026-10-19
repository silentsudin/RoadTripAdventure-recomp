// guest:0x00272a70 translated from Ghidra pseudocode and VU0 macro disassembly
#include "../guest_vu0.h"
namespace guest { using namespace rtguest::vu0;
void vu0SinCos() { S.vf[5]=lqc2(0x002e6e80); vmr32(0b0001,6,6); vaddbc(0b1000,0,4,0,6); vmul(0b1000,6,6,6); vmulbc(0b0111,0,4,4,0); vmulbc(0b1111,3,8,5,6); vsub(15,5,0,0); vmulbc(15,0,8,8,6); vmulbc(0b1110,0,8,8,6); vaddbc(0b1000,3,4,4,8); vmulbc(0b0110,0,8,8,6); vaddbc(0b1000,2,4,4,8); vmulbc(0b1000,0,8,8,6); vaddbc(0b1000,1,4,4,8); vaddbc(0b1000,0,4,4,8); vaddbc(0b1100,0,4,5,4); vmul(0b1000,7,4,4); vsubbc(0b0001,0,7,0,7); vsqrt(3,7); vwaitq(); vaddq(0b1000,7,0); vsubbc(0b1000,0,4,5,7); }
RT_GUEST_FN(0x00272a70, guest::vu0SinCos) }
