// guest:0x0026dc10 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t PrintCamProj(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  undefined8 uVar1;
uint32_t auStack_40 = rtguest::allocFrame(32);
  
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_40)), (char *)rtguest::G((uint32_t)(0x32fb98)), param_2);
  uiDebugText8(0x10,8,auStack_40,0xf);
  uVar1 = fptodp(param_1);
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_40)), (char *)rtguest::G((uint32_t)(0x335668)), uVar1);
  uiDebugText8(0x20,0x48,auStack_40,0xf);
  return 0;
}


RT_GUEST_FN(0x0026dc10, guest::PrintCamProj)

}  // namespace guest
