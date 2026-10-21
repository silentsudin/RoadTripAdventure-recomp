// guest:0x0026dc98 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t PrintUiSlotN(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x28, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
    rtguest::gS[0] = 0x0000000000330000ull;  // walk const
  }
  undefined8 uVar1;
uint32_t auStack_40 = rtguest::allocFrame(32);
  
  uVar1 = RT_CALLRA(0x26dcac, guest::fptodp());
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_40)), (char *)rtguest::G((uint32_t)(0x335670)), param_1, uVar1);
  RT_CALLRA(0x26dcd8, guest::uiDebugText8(0xa0,(int)param_1 << 3,auStack_40,0xf));
  return 0;
}


RT_GUEST_FN(0x0026dc98, guest::PrintUiSlotN)

}  // namespace guest
