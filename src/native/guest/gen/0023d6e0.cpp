// guest:0x0023d6e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetStateBitFc78AndFc88(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  uint uVar1;
  ulong uVar2;
  
  uVar1 = ((uint32_t)param_1 & 0xff) >> 6;
  uVar2 = 1L << (param_1 & 0x3f);
  (*(uint64_t*)rtguest::G((uint32_t)0x177fc78 + (uint32_t)(uVar1) * 8)) = (*(uint64_t*)rtguest::G((uint32_t)0x177fc78 + (uint32_t)(uVar1) * 8)) | uVar2;
  (*(uint64_t*)rtguest::G((uint32_t)0x177fc88 + (uint32_t)(uVar1) * 8)) = (*(uint64_t*)rtguest::G((uint32_t)0x177fc88 + (uint32_t)(uVar1) * 8)) | uVar2;
  return 0;
}


RT_GUEST_FN(0x0023d6e0, guest::SetStateBitFc78AndFc88)

}  // namespace guest
