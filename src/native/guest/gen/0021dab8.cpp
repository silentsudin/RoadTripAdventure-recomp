// guest:0x0021dab8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gifTexQuadTag(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(0);
  ulong uVar1;
  ulong uVar2;
  
  uVar1 = (*(uint64_t*)rtguest::G(0x29cc20));
  (*(uint64_t*)rtguest::G((uint32_t)param_1)) = (*(uint64_t*)rtguest::G(0x29cc20));
  uVar2 = (*(uint64_t*)rtguest::G(0x29cc28));
  (*(uint64_t*)rtguest::G((uint32_t)param_1)) = uVar1 & 0xffffffffffff8000 | 4;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint64_t))) = uVar2;
  return 0;
}


RT_GUEST_FN(0x0021dab8, guest::gifTexQuadTag)

}  // namespace guest
