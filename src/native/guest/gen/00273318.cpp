// guest:0x00273318 translated from Ghidra pseudocode and VU0 macro disassembly
#include "../guest_vu0.h"
namespace guest { using namespace rtguest::vu0; uint32_t sceVu0ClipScreen3(uint32_t a,uint32_t b,uint32_t c) { vsub(15,4,0,0); S.vf[6]=lqc2(a); S.vf[8]=lqc2(b); S.vf[9]=lqc2(c); qmtc2(7,{{4096,0,0,0}}); ctc2(16,0); vsub(0b1101,5,6,4); vsub(0b1100,5,7,6); vsub(0b1101,5,8,4); vsub(0b1100,5,7,8); vsub(0b1101,5,9,4); vsub(0b1100,5,7,9); return cfc2(16)&0xc0u; } RT_GUEST_FN(0x00273318, guest::sceVu0ClipScreen3) }
