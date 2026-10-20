// guest:0x00249520 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CalculateTachometerValue(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
  long lVar1;
  
  lVar1 = (long)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1d0)));
  if (((param_2 != 0) || (param_3 != 0)) && (10000 < lVar1)) {
    lVar1 = 10000;
  }
  if (param_2 != 0) {
    lVar1 = (long)(((int)lVar1 * (int)param_2 + 9999) / 10000);
  }
  return lVar1;
}


RT_GUEST_FN(0x00249520, guest::CalculateTachometerValue)

}  // namespace guest
