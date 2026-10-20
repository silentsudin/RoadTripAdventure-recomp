// guest:0x002068d0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiSpriteWriteWH(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, uint32_t param_7, ...) {
  rtguest::FrameGuard rt_fg;
  short sVar1;
  short sVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  iVar5 = (int)param_4;
  iVar3 = (param_3 << 0x11) >> 0x10;
  iVar6 = (int)param_5;
  iVar4 = (int)param_2;
  sVar1 = (short)((uint32_t)(param_3 << 0x11) >> 0x10);
  sVar2 = (short)((uint32_t)((int)param_7 << 0x11) >> 0x10);
  if ((*(uint32_t*)rtguest::G(0x333288)) == 0) {
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x22))) = (short)(iVar3 << 3) + 0x7900;
    (*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(uint64_t))) = (short)(iVar4 << 4) + 0x6c00;
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x42))) = (sVar1 + sVar2) * 8 + 0x7900;
    (*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(8) * sizeof(uint64_t))) = (param_2 + param_6) * 0x10 + 0x6c00;
  }
  else {
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x22))) = (short)(iVar3 << 3) + 0x7800;
    (*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(uint64_t))) = (short)((iVar4 << 6) / 5) + 0x7000;
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x42))) = (sVar1 + sVar2) * 8 + 0x7800;
    (*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(8) * sizeof(uint64_t))) = (short)(((iVar4 + param_6) * 0x40) / 5) + 0x7000;
  }
  iVar4 = iVar5 + param_6;
  iVar3 = iVar6 + param_7;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint64_t))) = (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint64_t))) & 0xffffffffc000c000 | (long)(iVar5 << 4) & 0x3ff0U |
               ((long)(iVar6 << 4) & 0x3ff0U) << 0x10;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(6) * sizeof(uint64_t))) = (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(6) * sizeof(uint64_t))) & 0xffffffffc000c000 | (long)(iVar4 * 0x10 + 0x10) & 0x3fffU |
               ((long)(iVar3 * 0x10 + 0x10) & 0x3fffU) << 0x10;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint64_t))) = 8;
  (*(uint64_t*)rtguest::G((uint32_t)param_1)) = (*(uint64_t*)rtguest::G((uint32_t)param_1)) & 0xfffff00000000000 | 10 | ((long)iVar5 & 0x3ffU) << 4 |
             ((long)(iVar4 + -1) & 0x3ffU) << 0xe | ((long)iVar6 & 0x3ffU) << 0x18 |
             ((long)(iVar3 + -1) & 0x3ffU) << 0x22;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x44))) = 0xfffffe;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(7) * sizeof(uint64_t))) = 3;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(9) * sizeof(uint64_t))) = 5;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint64_t))) = 3;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(5) * sizeof(uint64_t))) = 5;
  return 0;
}


RT_GUEST_FN(0x002068d0, guest::uiSpriteWriteWH)

}  // namespace guest
