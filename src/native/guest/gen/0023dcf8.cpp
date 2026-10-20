// guest:0x0023dcf8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetFlagFcb8(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  
  iVar1 = 9;
  if (param_1 != 0xd) {
    iVar1 = (int)param_1 + -1;
  }
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x0177fcb8 + iVar1 * 8))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x0177fcb8 + iVar1 * 8))) | 1L << param_2;
  return 0;
}


RT_GUEST_FN(0x0023dcf8, guest::SetFlagFcb8)

}  // namespace guest
