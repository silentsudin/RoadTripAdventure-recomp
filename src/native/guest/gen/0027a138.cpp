// guest:0x0027a138 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t mcHearAlarm(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  iWakeupThread(param_3);
  SYNC(0);
  EI();
  return 0;
}


RT_GUEST_FN(0x0027a138, guest::mcHearAlarm)

}  // namespace guest
