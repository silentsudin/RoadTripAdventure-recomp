// guest:0x00266b78 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t raceSummaryDrawRallyRow(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(192);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x90, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x98, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0xa8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0xa0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0xb0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xb4) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
    rtguest::gS[3] = 0x0000000001780000ull;  // walk const
  }
    uint32_t uStack_c0 = rtguest::allocFrame(4);
    uint32_t uStack_bc = rtguest::allocFrame(2);
  size_t sVar1;
  
  if (((*(uint32_t*)rtguest::G(0x178035c)) & 0x100000) == 0) {
    RT_CALLRA(0x266bc4, guest::uiDrawText(param_1,param_2 / 2,0x32f260,3));
    RT_CALLRA(0x266c04, guest::uiDrawText((int)param_1 + 0x14,(param_2 + 0x18) / 2,0x335310,3));
    if ((*(uint32_t*)rtguest::G(0x1780a10)) == 0x7fffffff) {
      (*(uint16_t*)rtguest::G((uint32_t)uStack_bc)) = (*(uint32_t*)rtguest::G(0x33531c));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_c0)) = (*(uint32_t*)rtguest::G(0x335318));
    }
    else {
      ::sprintf((char *)rtguest::G((uint32_t)(uStack_c0)), (char *)rtguest::G((uint32_t)(0x335320)), (*(uint32_t*)rtguest::G(0x1780a10)) / 9000, ((*(uint32_t*)rtguest::G(0x1780a10)) % 9000) / 0x96);
    }
    sVar1 = ::strlen((char *)rtguest::G((uint32_t)((uint32_t)uStack_c0)));
    RT_CALLRA(0x266c24, guest::uiDrawText((int)param_1 + (int)sVar1 * -0x10 + 0xf0,(param_2 + 0x18) / 2,uStack_c0,3));
  }
  else {
    RT_CALLRA(0x266cbc, guest::uiDrawText(param_1,param_2 / 2,0x32f250,3));
  }
  return param_2 + 0x48;
}


RT_GUEST_FN(0x00266b78, guest::raceSummaryDrawRallyRow)

}  // namespace guest
