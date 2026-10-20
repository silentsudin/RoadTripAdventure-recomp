// guest:0x0026dcf8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t PrintXyzRow(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(128);
  undefined8 uVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined8 uVar4;
uint32_t auStack_80 = rtguest::allocFrame(64);
  
  uVar1 = fptodp();
  uVar2 = fptodp(param_2);
  uVar3 = fptodp(param_3);
  uVar4 = fptodp(param_4);
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_80)), (char *)rtguest::G((uint32_t)(0x32fba8)), uVar1, uVar2, uVar3, uVar4);
  uiDebugText8(0x10,0x50,auStack_80,0xf);
  return 0;
}


RT_GUEST_FN(0x0026dcf8, guest::PrintXyzRow)

}  // namespace guest
