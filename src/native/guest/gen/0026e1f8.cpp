// guest:0x0026e1f8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t printFloat(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x48, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x50) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x54) = 0x00000000u;
    rtguest::gS[0] = 0x0000000000330000ull;  // walk const
  }
  undefined8 uVar1;
uint32_t auStack_60 = rtguest::allocFrame(64);
  
  uVar1 = RT_CALLRA(0x26e20c, guest::fptodp());
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_60)), (char *)rtguest::G((uint32_t)(0x335678)), uVar1);
  RT_CALLRA(0x26e234, guest::uiDebugText8(0x10,param_1 << 3,auStack_60,0xf));
  return 0;
}


RT_GUEST_FN(0x0026e1f8, guest::printFloat)

}  // namespace guest
