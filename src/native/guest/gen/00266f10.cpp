// guest:0x00266f10 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t raceSummaryDrawFigure8Row(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
uint32_t auStack_80 = rtguest::allocFrame(48);
  
  iVar1 = param_2 + 0xa8;
  if (((*(uint32_t*)rtguest::G(0x1780344)) & 0x40) == 0) {
    uiDrawText(param_1,param_2 / 2,(*(uint32_t*)rtguest::G(0x002be3fc)),3);
    uVar4 = 0;
    do {
      iVar1 = param_2;
      param_2 = iVar1 + 0x18;
      uVar3 = uVar4 + 1;
      iVar2 = param_2 / 2;
      ::sprintf((char *)rtguest::G((uint32_t)(auStack_80)), (char *)rtguest::G((uint32_t)(0x32f270)), uVar3);
      uiDrawText((int)param_1 + 0x14,iVar2,auStack_80,3);
      uiDrawTime((int)param_1 + 0xa0,iVar2,(*(uint32_t*)rtguest::G((uint32_t)0x1780a14 + (uint32_t)(uVar4) * 4)),3);
      uVar4 = uVar3;
    } while (uVar3 < 5);
    iVar1 = iVar1 + 0x48;
  }
  else {
    uiDrawText(param_1,param_2 / 2,0x32f250,3);
  }
  return iVar1;
}


RT_GUEST_FN(0x00266f10, guest::raceSummaryDrawFigure8Row)

}  // namespace guest
