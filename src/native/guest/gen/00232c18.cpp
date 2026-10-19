// guest:0x00232c18 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t raceHudUpdate(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  undefined4 uVar1;
uint32_t __s;
  uint uVar2;
  
  param_1 = param_1 + (int)param_2 * 0x408;
  __s = (uint32_t)(param_1 + 0x8538);
  uVar2 = (uint32_t)(param_3 != 0);
  if ((*(uint32_t*)rtguest::G((uint32_t)__s)) != uVar2) {
    if (param_2 == 0) {
      (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x2c));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8540))) = 0x87;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x854c))) = 0x0029f768;
      (*(uint32_t*)rtguest::G((uint32_t)__s)) = uVar2;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x853c))) = 0x140;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8544))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8548))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8550))) = 1;
      (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)((uint32_t)(param_1 + 0x8564))), 0, 0x2c));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8564))) = uVar2;
      uVar1 = 0x5c;
    }
    else {
      (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x2c));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8540))) = 0x167;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x854c))) = 0x0029f768;
      (*(uint32_t*)rtguest::G((uint32_t)__s)) = uVar2;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x853c))) = 0x140;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8544))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8548))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8550))) = 1;
      (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)((uint32_t)(param_1 + 0x8564))), 0, 0x2c));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8564))) = uVar2;
      uVar1 = 0x13c;
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x857c))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8568))) = 0x140;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x856c))) = uVar1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8574))) = 5;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8578))) = 0x0029f788;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8570))) = 1;
  }
  return 0;
}


RT_GUEST_FN(0x00232c18, guest::raceHudUpdate)

}  // namespace guest
