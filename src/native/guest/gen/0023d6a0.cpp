// guest:0x0023d6a0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t TestStateBitFc88(uint32_t param_1, ...) {
  uint uVar1;
  ulong uVar2;
  
  uVar1 = (param_1 & 0xff) + 0xff;
  uVar2 = (uint32_t)(int)uVar1;
  if ((param_1 & 0xff) != 0) {
    uVar2 = (uint32_t)(((*(uint64_t*)rtguest::G((uint32_t)0x177fc88 + (uint32_t)((uVar1 & 0xff) >> 6) * 8)) & 1L << (uVar2 & 0x3f)) != 0);
  }
}


RT_GUEST_FN(0x0023d6a0, guest::TestStateBitFc88)

}  // namespace guest
