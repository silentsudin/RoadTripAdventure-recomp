// guest:0x0020c7b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// 2026-10-19 MIPS cross-check 0x0020c7b8: Ghidra halved chunk walk (qword +0x10 / stride size*8+8); real MIPS qword +0x20, stride size*16+0x10 (see gen/0020dde0.cpp).
uint32_t loadCarModel(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  char cVar1;
uint32_t puVar2;
  undefined8 uVar3;
  ulong uVar4;
  int iVar5;
  int iVar6;
uint32_t puVar7;
  
  if (((*(uint32_t*)rtguest::G(0x3356c0)) & 1) == 0) {
    cdReadEntry(0x29b438 + (int)param_3 * 0xc,0x100000);
    (*(uint32_t*)rtguest::G(0x3356c0)) = (*(uint32_t*)rtguest::G(0x3356c0)) | 0x8000;
  }
  else if (((*(uint32_t*)rtguest::G(0x3356c0)) & 1) == 1) {
    if (0x34000 < (*(int32_t*)rtguest::G(0x100018))) {
      fatal(0x2ea638,param_3,(*(int32_t*)rtguest::G(0x100018)));
    }
    (uint32_t)(uintptr_t)(::memmove((char *)rtguest::G((uint32_t)(param_1)), (char *)rtguest::G((uint32_t)(0x00100000)), (long)(*(int32_t*)rtguest::G(0x100018))));
    iVar5 = 0;
    do {
      iVar6 = iVar5 + 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x177f310 + (uint32_t)(param_2 * 6 + iVar5) * 4)) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 * 4 + (int)param_1))) + (int)param_1;
      iVar5 = iVar6;
    } while (iVar6 < 6);
    puVar7 = (uint32_t)((*(int32_t*)rtguest::G(0x100018)) + 0x100000);
    param_1 = (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x18))) + (int)param_1);
    cVar1 = (*(uint8_t*)rtguest::G((uint32_t)0x100003 + (uint32_t)((*(int32_t*)rtguest::G(0x100018))) * 1));
    puVar2 = puVar7;
    while (cVar1 != 'p') {
      if (cVar1 == '\x10') {
        uVar4 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar2 + 0x20)));
      }
      else {
        fatal(0x2ea490);
        uVar4 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar2 + 0x20)));
      }
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar2 + 0x20))) =
           uVar4 & 0xffffc000ffffffff |
           ((long)(int)(((uint32_t)(uVar4 >> 0x20) & 0x3fff) +
                       ((param_2 * 0x44 + 0x243e) * 0x10000 >> 0x10)) & 0x3fffU) << 0x20;
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar2 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar2)) * 0x10 + 0x13)));
      puVar2 = puVar2 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar2)) * 0x10 + 0x10;
    }
    uVar3 = sceDmaGetChan(1);
    sceDmaSync(uVar3,0,0);
    sendTexturePackets(puVar7);
  }
  return param_1;
}


RT_GUEST_FN(0x0020c7b8, guest::loadCarModel)

}  // namespace guest
