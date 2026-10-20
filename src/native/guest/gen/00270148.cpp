// guest:0x00270148 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t memclr(uint32_t param_1, uint64_t param_2, ...) {
  rtguest::FrameGuard rt_fg(0);
  int iVar1;
  
  iVar1 = (int)param_2 + -1;
  if (param_2 != 0) {
    do {
      (*(uint8_t*)rtguest::G((uint32_t)param_1)) = 0;
      iVar1 = iVar1 + -1;
      param_1 = param_1 + 1;
    } while (iVar1 != -1);
  }
  return 0;
}


RT_GUEST_FN(0x00270148, guest::memclr)

}  // namespace guest
