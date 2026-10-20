// guest:0x0026e158 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t printXyzFixed(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
uint32_t auStack_50 = rtguest::allocFrame(64);
  
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)param_1));
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t)));
  iVar4 = (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(int32_t)));
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int32_t)));
  if (iVar1 < 0) {
    iVar1 = iVar1 + 0xf;
  }
  if (iVar2 < 0) {
    iVar2 = iVar2 + 0xf;
  }
  if (iVar3 < 0) {
    iVar3 = iVar3 + 0xf;
  }
  if (iVar4 < 0) {
    iVar4 = iVar4 + 0xf;
  }
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_50)), (char *)rtguest::G((uint32_t)(0x32fc30)), (iVar1 >> 4) + -0x6c0, (iVar2 >> 4) + -0x790, iVar3 >> 4, iVar4 >> 4);
  uiDebugText8(0x10,param_2 << 3,auStack_50,0xf);
  return 0;
}


RT_GUEST_FN(0x0026e158, guest::printXyzFixed)

}  // namespace guest
