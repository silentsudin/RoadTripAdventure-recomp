// guest:0x00274548 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t scePadExitPressMode(uint64_t param_1, uint32_t param_2, ...) {
  undefined8 uVar1;
  
  if ((*(uint32_t*)rtguest::G((uint32_t)0x1773220 + (uint32_t)((int)param_1 * 0x1c + param_2 * 7) * 4)) == 0) {
    uVar1 = 0;
  }
  else {
    uVar1 = scePadSetButtonInfo(param_1,param_2,0);
  }
  return uVar1;
}


RT_GUEST_FN(0x00274548, guest::scePadExitPressMode)

}  // namespace guest
