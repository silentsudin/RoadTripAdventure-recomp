// guest:0x0024f4e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CountDecimalDigits(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  int iVar2;
  
  iVar1 = 1;
  iVar2 = (int)param_1;
  while (iVar2 = iVar2 / 10, iVar2 != 0) {
    iVar1 = iVar1 + 1;
  }
  return iVar1;
}


RT_GUEST_FN(0x0024f4e0, guest::CountDecimalDigits)

}  // namespace guest
