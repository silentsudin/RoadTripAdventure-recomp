// guest:0x00272a70 translated from Ghidra pseudocode and VU0 macro disassembly
#include "../guest_vu0.h"
namespace guest { using namespace rtguest::vu0;
uint32_t g_a3SinCos = 0; /* $a3 handed in by vu0RotMatrixX/Y/Z */
void vu0SinCos() { S.vf[5]=lqc2(0x002e6e80); vmr32(0b0001,6,6); vaddbc(0b1000,0,4,0,6); vmul(0b1000,6,6,6); vmulbc(0b0111,0,4,4,0); vmulbc(0b1111,3,8,5,6); vsub(15,5,0,0); vmulbc(15,0,8,8,6); vmulbc(0b1110,0,8,8,6); vaddbc(0b1000,3,4,4,8); vmulbc(0b1100,0,8,8,6) /* 0x272aa0 word 4b864218: dest=xy (1100), was 0110 */; vaddbc(0b1000,2,4,4,8); vmulbc(0b1000,0,8,8,6); vaddbc(0b1000,1,4,4,8); vaddbc(0b1000,0,4,4,8); vaddbc(0b1100,0,4,5,4); vmul(0b1000,7,4,4); vsubbc(0b0001,0,7,0,7); vsqrt(3,7); vwaitq(); vaddq(0b1000,7,0); if (g_a3SinCos == 0) vaddbc(0b1000,0,4,5,7); else vsubbc(0b1000,0,4,5,7); /* fn/00272a70.c:42 `if (in_a3 == 0) _vaddbc else _vsubbc`; MIPS 272ac8 bnez $7; $a3 = 1 iff caller angle < 0 (272bb4/bbc) */ }
RT_GUEST_FN(0x00272a70, guest::vu0SinCos) }
