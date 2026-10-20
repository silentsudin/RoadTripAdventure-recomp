// guest:0x0023ddb8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t FindCurrentTaskIndex(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  
  iVar1 = 0;
  if ((long)(*(int32_t*)rtguest::G(0x335734)) != (long)(*(uint32_t*)rtguest::G(0x2a2b30))) {
    for (iVar1 = 1; (iVar1 < 6 && ((long)(*(int32_t*)rtguest::G(0x335734)) != (long)(*(uint32_t*)rtguest::G((uint32_t)0x2a2b30 + (uint32_t)(iVar1) * 4))));
        iVar1 = iVar1 + 1) {
    }
  }
  return iVar1;
}


RT_GUEST_FN(0x0023ddb8, guest::FindCurrentTaskIndex)

}  // namespace guest
