// guest:0x0027a160 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t mcDelayThread(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  undefined8 uVar1;
  
  uVar1 = GetThreadId();
  SetAlarm(param_1,0x27a138,uVar1);
  SleepThread();
  return 0;
}


RT_GUEST_FN(0x0027a160, guest::mcDelayThread)

}  // namespace guest
