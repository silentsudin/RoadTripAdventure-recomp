// guest:0x0027a160 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t mcDelayThread(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
    rtguest::gS[0] = 0x0000000000280000ull;  // walk const
  }
  undefined8 uVar1;
  
  uVar1 = RT_CALLRA(0x27a178, guest::GetThreadId());
  RT_CALLRA(0x27a188, guest::SetAlarm(param_1,0x27a138,uVar1));
  SleepThread();
  return 0;
}


RT_GUEST_FN(0x0027a160, guest::mcDelayThread)

}  // namespace guest
