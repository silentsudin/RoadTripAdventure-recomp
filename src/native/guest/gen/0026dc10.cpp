// guest:0x0026dc10 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t PrintCamProj(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x28) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x2c) = 0x00000000u;
  }
  undefined8 uVar1;
uint32_t auStack_40 = rtguest::allocFrame(32);
  
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_40)), (char *)rtguest::G((uint32_t)(0x32fb98)), param_2);
  RT_CALLRA(0x26dc4c, guest::uiDebugText8(0x10,8,auStack_40,0xf));
  uVar1 = RT_CALLRA(0x26dc54, guest::fptodp(param_1));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_40)), (char *)rtguest::G((uint32_t)(0x335668)), uVar1);
  RT_CALLRA(0x26dc78, guest::uiDebugText8(0x20,0x48,auStack_40,0xf));
  return 0;
}


RT_GUEST_FN(0x0026dc10, guest::PrintCamProj)

}  // namespace guest
