// guest:0x0020cc30 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CheckInputMapping(uint32_t param_1, uint32_t param_2, ...) {
  undefined1 uVar1;
  uint uVar2;
  long lVar3;
  
  if ((*(uint32_t*)rtguest::G(0x3356c0)) == 1) {
    uVar2 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26)));
    uVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27)));
  }
  else {
    if (1 < (*(uint32_t*)rtguest::G(0x3356c0))) {
      if ((*(uint32_t*)rtguest::G(0x3356c0)) != 2) {
        return false;
      }
      (*(uint32_t*)rtguest::G(0x3356c0)) = 0xffffffff;
      (*(uint8_t*)rtguest::G((uint32_t)0x335720 + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) >> 8 & 7) * 1)) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27)));
    }
    if ((*(uint32_t*)rtguest::G(0x3356c0)) != 0) {
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x60))) = 0xffffffff;
    uVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) = uVar1;
    uVar2 = ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) >> 8 & 7U) + 0xf;
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) = (char)uVar2;
  }
  lVar3 = loadCarModel(0x3fbb80 + uVar2 * 0x34000,uVar2,uVar1);
  return lVar3 == 0;
}


RT_GUEST_FN(0x0020cc30, guest::CheckInputMapping)

}  // namespace guest
