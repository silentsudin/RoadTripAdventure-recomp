// guest:0x0026e100 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t printHexVec4(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x48) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4c) = 0x00000000u;
  }
uint32_t auStack_50 = rtguest::allocFrame(64);
  
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_50)), (char *)rtguest::G((uint32_t)(0x32fc20)), (*(uint32_t*)rtguest::G((uint32_t)param_1)), (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint32_t))), (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))), (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint32_t))));
  RT_CALLRA(0x26e140, guest::uiDebugText8(0x10,param_2 << 3,auStack_50,0xf));
  return 0;
}


RT_GUEST_FN(0x0026e100, guest::printHexVec4)

}  // namespace guest
