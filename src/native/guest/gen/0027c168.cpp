// guest:0x0027c168 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t decode_uleb128(uint32_t param_1, uint32_t param_2, ...) {
  byte bVar1;
  uint uVar2;
  uint uVar3;
  
  bVar1 = (*(int8_t*)rtguest::G((uint32_t)param_1));
  uVar3 = 0;
  uVar2 = bVar1 & 0x7f;
  while (param_1 = param_1 + 1, (bVar1 & 0x80) != 0) {
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)param_1));
    uVar3 = uVar3 + 7;
    uVar2 = uVar2 | (bVar1 & 0x7f) << (uVar3 & 0x1f);
  }
  (*(uint32_t*)rtguest::G((uint32_t)param_2)) = uVar2;
  return param_1;
}


RT_GUEST_FN(0x0027c168, guest::decode_uleb128)

}  // namespace guest
