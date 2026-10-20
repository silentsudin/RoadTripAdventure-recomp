// guest:0x00266df8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t raceSummaryDrawCurlingRow(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(96);
  size_t sVar1;
  int iVar2;
uint32_t acStack_60 = rtguest::allocFrame(48);
  
  if (((*(uint32_t*)rtguest::G(0x1780350)) & 0x40000) == 0) {
    uiDrawText(param_1,param_2 / 2,(*(uint32_t*)rtguest::G(0x002be418)),3);
    iVar2 = (param_2 + 0x18) / 2;
    uiDrawText((int)param_1 + 0x14,iVar2,0x335310,3);
    ::sprintf((char *)rtguest::G((uint32_t)(acStack_60)), (char *)rtguest::G((uint32_t)(0x335330)), (*(uint32_t*)rtguest::G(0x1780a2c)));
    sVar1 = ::strlen((char *)rtguest::G((uint32_t)(acStack_60)));
    uiDrawText((int)param_1 + (int)sVar1 * -10 + 0xf0,iVar2,acStack_60,3);
  }
  else {
    uiDrawText(param_1,param_2 / 2,0x32f250,3);
  }
  return param_2 + 0x48;
}


RT_GUEST_FN(0x00266df8, guest::raceSummaryDrawCurlingRow)

}  // namespace guest
