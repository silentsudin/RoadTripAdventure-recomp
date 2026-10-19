// guest:0x00243a00 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetPartValueU16(uint32_t param_1, uint32_t param_2, ...) {
  undefined2 uVar1;
  
  if (param_1 == 0xb) {
    return (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x002f03ee + param_2 * 0x14)));
  }
  if (param_1 < 0xc) {
    if (param_1 != 8) {
      return 0;
    }
    uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x002eff4e + param_2 * 0x10)));
  }
  else {
    uVar1 = 0xb;
    if (param_1 != 0xc) {
    }
  }
}


RT_GUEST_FN(0x00243a00, guest::GetPartValueU16)

}  // namespace guest
