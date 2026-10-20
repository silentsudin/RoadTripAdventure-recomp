// guest:0x0021dae8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gifSetSpriteCount(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  ulong uVar1;
  ulong uVar2;
  
  uVar1 = (*(uint64_t*)rtguest::G(0x29cbe0));
  (*(uint64_t*)rtguest::G((uint32_t)param_1)) = (*(uint64_t*)rtguest::G(0x29cbe0));
  uVar2 = (*(uint64_t*)rtguest::G(0x29cbe8));
  (*(uint64_t*)rtguest::G((uint32_t)param_1)) = uVar1 & 0xffffffffffff8000 | (long)(param_2 * 5) & 0x7fffU;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint64_t))) = uVar2;
  return 0;
}


RT_GUEST_FN(0x0021dae8, guest::gifSetSpriteCount)

}  // namespace guest
