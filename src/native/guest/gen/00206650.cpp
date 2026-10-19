// guest:0x00206650 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiSpriteWriteZ(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, uint32_t param_7, uint32_t param_8, ...) {
  short sVar1;
  short sVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  iVar4 = (int)param_6;
  iVar5 = (int)param_7;
  sVar2 = param_4 - param_6;
  if (-1 < iVar4) {
    sVar2 = param_4;
  }
  sVar1 = param_5 - param_7;
  if (-1 < iVar5) {
    sVar1 = param_5;
  }
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint64_t))) = (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint64_t))) & 0xffffffffc000c000 | (long)((int)sVar2 << 4) & 0x3ff0U |
               ((long)((int)sVar1 << 4) & 0x3ff0U) << 0x10;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(6) * sizeof(uint64_t))) = (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(6) * sizeof(uint64_t))) & 0xffffffffc000c000 | (long)((sVar2 + iVar4) * 0x10 + 0x10) & 0x3fffU |
               ((long)((sVar1 + iVar5) * 0x10 + 0x10) & 0x3fffU) << 0x10;
  iVar3 = (int)param_2;
  if (iVar4 < 0) {
    iVar4 = iVar4 * -0x10000 >> 0x10;
  }
  if (iVar5 < 0) {
    iVar5 = iVar5 * -0x10000 >> 0x10;
  }
  if ((*(uint32_t*)rtguest::G(0x333288)) == 0) {
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x22))) = (short)((int)param_3 << 3) + 0x7900;
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 4))) = (short)(iVar3 << 4) + 0x6c00;
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x42))) = (param_3 + (short)iVar5) * 8 + 0x7900;
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) = (param_2 + (short)iVar4) * 0x10 + 0x6c00;
  }
  else {
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x22))) = (short)((int)param_3 << 3) + 0x7800;
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 4))) = (short)((iVar3 << 6) / 5) + 0x7000;
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x42))) = (param_3 + (short)iVar5) * 8 + 0x7800;
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) = (short)(((iVar3 + iVar4) * 0x40) / 5) + 0x7000;
  }
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint64_t))) = 8;
  (*(uint64_t*)rtguest::G((uint32_t)param_1)) = (*(uint64_t*)rtguest::G((uint32_t)param_1)) & 0xfffff00000000000 | 10 | ((long)(int)param_4 & 0x3ffU) << 4 |
             ((long)(param_4 + iVar4 + -1) & 0x3ffU) << 0xe | ((long)(int)param_5 & 0x3ffU) << 0x18
             | ((long)(param_5 + iVar5 + -1) & 0x3ffU) << 0x22;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x44))) = param_8;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(7) * sizeof(uint64_t))) = 3;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(9) * sizeof(uint64_t))) = 5;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint64_t))) = 3;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(5) * sizeof(uint64_t))) = 5;
  return 0;
}


RT_GUEST_FN(0x00206650, guest::uiSpriteWriteZ)

}  // namespace guest
