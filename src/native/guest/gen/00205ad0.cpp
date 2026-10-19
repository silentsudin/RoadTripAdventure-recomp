// guest:0x00205ad0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CenterRectangle(uint32_t param_1, uint32_t param_2, ...) {
  ushort uVar1;
  short sVar2;
  uint uVar3;
  int iVar4;
  
  uVar3 = param_2 & 3;
  if (uVar3 == 1) {
    sVar2 = 0x34;
  }
  else {
    if (uVar3 < 2) {
      if (uVar3 == 0) {
        (*(int16_t*)rtguest::G((uint32_t)param_1)) = 0x140 - (short)((int)((int)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int16_t))) + (uint32_t)((uint32_t)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int16_t))) >> 0xf)) >> 1);
      }
      goto LAB_00205b50;
    }
    if (uVar3 != 2) goto LAB_00205b50;
    sVar2 = 0x24c - (*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int16_t)));
  }
  (*(int16_t*)rtguest::G((uint32_t)param_1)) = sVar2;
LAB_00205b50:
  param_2 = param_2 & 0xc;
  if (param_2 == 4) {
    (*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int16_t))) = 0x18;
    return 0;
  }
  if (param_2 < 5) {
    if (param_2 == 0) {
      (*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int16_t))) = 0x70 - (short)((int)((int)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(int16_t))) + (uint32_t)((uint32_t)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(int16_t))) >> 0xf)) >> 1);
    }
  }
  else {
    if (param_2 != 8) {
    }
    uVar1 = (*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(int16_t)));
    iVar4 = 0xa8 - ((int)((int)(short)uVar1 + (uint32_t)(uVar1 >> 0xf)) >> 1);
    (*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int16_t))) = (short)iVar4;
    if (0xd8 < (iVar4 * 0x10000 >> 0x10) + (int)(short)uVar1) {
      (*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int16_t))) = 0xd8 - uVar1;
    }
  }
}


RT_GUEST_FN(0x00205ad0, guest::CenterRectangle)

}  // namespace guest
