// guest:0x0023de50 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t TestProgressFlagForPair(uint32_t param_1, uint32_t param_2, ...) {
  uint uVar1;
uint32_t pcVar2;
  
  pcVar2 = (uint32_t)0x2a2b71;
  uVar1 = 0;
  while ((param_1 != (*(int8_t*)rtguest::G((uint32_t)pcVar2 + (uint32_t)(-1) * sizeof(int8_t))) || (param_2 != (*(int8_t*)rtguest::G((uint32_t)pcVar2))))) {
    uVar1 = uVar1 + 1;
    pcVar2 = pcVar2 + 2;
    if (0x18 < (int)uVar1) {
      return false;
    }
  }
  return ((*(uint32_t*)rtguest::G(0x177fdd0)) & 1 << (uVar1 & 0x1f)) != 0;
}


RT_GUEST_FN(0x0023de50, guest::TestProgressFlagForPair)

}  // namespace guest
