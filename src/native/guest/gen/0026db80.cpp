// guest:0x0026db80 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t PrintCamPair(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x28) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x2c) = 0x00000000u;
  }
  undefined8 uVar1;
uint32_t auStack_40 = rtguest::allocFrame(32);
  
  uVar1 = RT_CALLRA(0x26db98, guest::fptodp());
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_40)), (char *)rtguest::G((uint32_t)(0x32fb78)), uVar1);
  RT_CALLRA(0x26dbc0, guest::uiDebugText8(0x10,0x88,auStack_40,0xf));
  uVar1 = RT_CALLRA(0x26dbc8, guest::fptodp(param_2));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_40)), (char *)rtguest::G((uint32_t)(0x32fb88)), uVar1);
  RT_CALLRA(0x26dbf0, guest::uiDebugText8(0x10,0x90,auStack_40,0xf));
  return 0;
}


RT_GUEST_FN(0x0026db80, guest::PrintCamPair)

}  // namespace guest
