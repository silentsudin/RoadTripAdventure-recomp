// guest:0x002732d0 translated from Ghidra pseudocode and VU0 macro disassembly
#include "../guest_vu0.h"
namespace guest { using namespace rtguest::vu0; uint32_t sceVu0ClipScreen(uint32_t p) { vsub(15,4,0,0); S.vf[7]=lqc2(p); qmtc2(6,{{4096,0,0,0}}); ctc2(16,0); vsub(0b1101,5,7,4); vsub(0b1100,5,6,7); return cfc2(16)&0xc0u; } RT_GUEST_FN(0x002732d0, guest::sceVu0ClipScreen) }
