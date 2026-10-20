// guest:0x0026e808 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceGsSetDefLoadImage(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, uint32_t param_7, uint32_t param_8, ...) {
  rtguest::FrameGuard rt_fg;
  undefined4 uVar1;
  ulong uVar2;
  int iVar3;
  int iVar4;
  
  iVar4 = (int)param_7;
  iVar3 = (int)param_8;
  uVar2 = 0;
  switch(param_4) {
  case 0:
  case 0x30:
    uVar2 = (uint32_t)(iVar4 * iVar3 >> 2);
    break;
  case 1:
  case 0x31:
    uVar2 = (uint32_t)(iVar4 * iVar3 * 3 >> 4);
    break;
  case 2:
  case 10:
  case 0x32:
  case 0x3a:
    uVar2 = (uint32_t)(iVar4 * iVar3 >> 3);
    break;
  case 0x13:
  case 0x1b:
    uVar2 = (uint32_t)(iVar4 * iVar3 >> 4);
    break;
  case 0x14:
  case 0x24:
  case 0x2c:
    uVar2 = (uint32_t)(iVar4 * iVar3 >> 5);
  }
  if ((long)uVar2 < 0x8000) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x50))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x54))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x58))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x5c))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)param_1)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 4))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xc))) = 0;
    (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(10) * sizeof(uint64_t))) = (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(10) * sizeof(uint64_t))) & 0xf3ffffffffff8000 | uVar2 & 0x7fff | 0x800000000008000;
    (*(uint64_t*)rtguest::G((uint32_t)param_1)) = (*(uint64_t*)rtguest::G((uint32_t)param_1)) & 0xfffffffffff8000 | 0x1000000000000004;
    (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint64_t))) = (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint64_t))) & 0xfffffffffffffff0 | 0xe;
    (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint64_t))) = ((long)(int)param_2 << 0x30) >> 0x10 | (long)(int)param_3 << 0x30 |
                 (long)(int)param_4 << 0x38;
    (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint64_t))) = 0x50;
    (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(uint64_t))) = ((long)(int)param_5 << 0x30) >> 0x10 | (long)(int)param_6 << 0x30;
    (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(5) * sizeof(uint64_t))) = 0x51;
    (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(6) * sizeof(uint64_t))) = (long)param_7 | ((long)iVar3 << 0x30) >> 0x10;
    (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(7) * sizeof(uint64_t))) = 0x52;
    (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(9) * sizeof(uint64_t))) = 0x53;
    (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(8) * sizeof(uint64_t))) = 0;
    SYNC(0);
    uVar1 = 6;
  }
  else {
    rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x32fde0u)));
    uVar1 = 0;
  }
  return uVar1;
}


RT_GUEST_FN(0x0026e808, guest::sceGsSetDefLoadImage)

}  // namespace guest
