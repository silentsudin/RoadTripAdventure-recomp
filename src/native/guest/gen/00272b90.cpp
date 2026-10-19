// guest:0x00272b90 translated from Ghidra pseudocode and VU0 macro disassembly
#include "../guest_vu0.h"
namespace guest { using namespace rtguest::vu0; void vu0SinCos();
void vu0RotMatrixX(float a,uint32_t o,uint32_t i) { qmtc2(6,{{a<0?a+1.5707964f:1.5707964f-a,0,0,0}}); vu0SinCos(); vmove(15,6,5); vmove(15,7,5); vmove(15,8,5); vmove(15,9,5); vaddbc(0b1000,3,6,5,0); vaddbc(0b0001,3,9,5,0); vsub(0b0011,4,4,4); vaddbc(0b0010,0,7,5,4); vaddbc(0b0100,1,7,5,4); vsubbc(0b0100,0,8,5,4); vaddbc(0b0010,1,8,5,4); for(int n=0;n<4;++n){S.vf[4]=lqc2(i+16u*n);vmulabc(15,0,6,4);vmaddabc(15,1,7,4);vmaddabc(15,2,8,4);vmaddbc(15,3,5,9,4);sqc2(o+16u*n,S.vf[5]);} }
RT_GUEST_FN(0x00272b90, guest::vu0RotMatrixX) }
