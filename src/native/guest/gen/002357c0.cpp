// guest:0x002357c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DispatchTypeSetup(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  switch((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x22)))) {
  case 0x12:
  case 0x13:
  case 0x1b:
  case 0x1f:
  case 0x21:
  case 0x25:
    UpdateTimedPhaseTask(param_1,param_2,0,0,0);
    return 0;
  case 0x14:
  case 0x15:
  case 0x1d:
    UpdateTimedPhaseTask(param_1,param_2,2,0,0);
    return 0;
  default:
    RunCountdownTimerTask();
    return 0;
  case 0x18:
    UpdateTimedPhaseTask(param_1,param_2,0,1,1);
    return 0;
  case 0x1e:
    UpdateTimedPhaseTask(param_1,param_2,3,0,0);
    return 0;
  case 0x20:
  case 0x24:
    UpdateTimedPhaseTask(param_1,param_2,1,0,0);
    return 0;
  }
}


RT_GUEST_FN(0x002357c0, guest::DispatchTypeSetup)

}  // namespace guest
