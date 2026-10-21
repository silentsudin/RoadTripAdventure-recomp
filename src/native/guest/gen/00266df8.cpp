// guest:0x00266df8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t raceSummaryDrawCurlingRow(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x38, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x48, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x30, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x50) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x54) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(2, (uint32_t)param_1);  // walk: = param_1
    rtguest::gS[3] = 0x0000000001780000ull;  // walk const
  }
  size_t sVar1;
  int iVar2;
uint32_t acStack_60 = rtguest::allocFrame(48);
  
  if (((*(uint32_t*)rtguest::G(0x1780350)) & 0x40000) == 0) {
    RT_CALLRA(0x266e44, guest::uiDrawText(param_1,param_2 / 2,(*(uint32_t*)rtguest::G(0x002be418)),3));
    iVar2 = (param_2 + 0x18) / 2;
    RT_CALLRA(0x266e88, guest::uiDrawText((int)param_1 + 0x14,iVar2,0x335310,3));
    ::sprintf((char *)rtguest::G((uint32_t)(acStack_60)), (char *)rtguest::G((uint32_t)(0x335330)), (*(uint32_t*)rtguest::G(0x1780a2c)));
    sVar1 = ::strlen((char *)rtguest::G((uint32_t)(acStack_60)));
    RT_CALLRA(0x266ea4, guest::uiDrawText((int)param_1 + (int)sVar1 * -10 + 0xf0,iVar2,acStack_60,3));
  }
  else {
    RT_CALLRA(0x266ee8, guest::uiDrawText(param_1,param_2 / 2,0x32f250,3));
  }
  return param_2 + 0x48;
}


RT_GUEST_FN(0x00266df8, guest::raceSummaryDrawCurlingRow)

}  // namespace guest
