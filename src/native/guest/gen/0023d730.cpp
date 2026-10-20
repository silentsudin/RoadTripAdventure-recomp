// guest:0x0023d730 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetStateBitFc88(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  uint uVar1;
  
  uVar1 = (param_1 & 0xff) + 0xff;
  if ((param_1 & 0xff) != 0) {
    (*(uint64_t*)rtguest::G((uint32_t)0x177fc88 + (uint32_t)((uVar1 & 0xff) >> 6) * 8)) =
         (*(uint64_t*)rtguest::G((uint32_t)0x177fc88 + (uint32_t)((uVar1 & 0xff) >> 6) * 8)) | 1L << ((long)(int)uVar1 & 0x3fU);
  }
  return 0;
}


RT_GUEST_FN(0x0023d730, guest::SetStateBitFc88)

}  // namespace guest
