// guest:0x0023c418 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetObjectAttribute(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  ushort uVar1;
  int iVar2;
  
  (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + param_2 + 0xc))) = (char)param_3;
  iVar2 = (int)param_3;
  switch(param_2) {
  case 1:
    if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(0x002eca8a + iVar2 * 0x1c))) != 0) {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) | 0x400;
      return 0;
    }
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) & 0xfbff;
  case 2:
    uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) & 0xfffc;
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ecee4 + iVar2 * 0x18)));
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) = uVar1;
    if (iVar2 == 1) {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) = uVar1 | 1;
    }
    if (iVar2 == 2) {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) = uVar1 | 2;
    }
    break;
  case 7:
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 6))) = (char)param_3;
  case 8:
    if (param_3 != 0) {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) | 0x10;
    }
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) & 0xffef;
  case 9:
    if (param_3 != 0) {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) | 0x1000;
    }
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) & 0xefff;
  case 10:
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) =
         (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) & 0xdfbf | (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x002f0304 + iVar2 * 0x10)));
  case 0xb:
    uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x002f03ec + iVar2 * 0x14)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 7))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(0x002f03f0 + iVar2 * 5)));
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) & 0xfcdb | uVar1;
  case 0xc:
    if (param_3 != 0) {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) | 0x4000;
    }
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) & 0xbfff;
  }
}


RT_GUEST_FN(0x0023c418, guest::SetObjectAttribute)

}  // namespace guest
