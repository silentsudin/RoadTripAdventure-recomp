// guest:0x0020c9d0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t loadCarModelAt(uint32_t param_1, ...) {
  byte bVar1;
  char cVar2;
uint32_t puVar3;
  int iVar4;
  uint uVar5;
  undefined4 uVar6;
  undefined8 uVar7;
  ulong uVar8;
  int iVar9;
uint32_t __dest;
  int iVar10;
uint32_t puVar11;
  
  iVar9 = (*(int32_t*)rtguest::G(0x100004));
  if ((*(uint32_t*)rtguest::G(0x3356c0)) == 1) {
    ::printf((char *)rtguest::G((uint32_t)(0x2ea650u)), (*(int32_t*)rtguest::G(0x100004)));
    if (iVar9 < 0x34001) {
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26)));
    }
    else {
      fatal(0x2ea660,iVar9);
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26)));
    }
    uVar5 = (uint32_t)bVar1;
    iVar10 = (uint32_t)bVar1 * 2 + uVar5;
    iVar9 = (iVar10 * 4 + uVar5) * 0x4000;
    __dest = (uint32_t)(0x3fbb80 + iVar9);
    (uint32_t)(uintptr_t)(::memmove((char *)rtguest::G((uint32_t)(__dest)), (char *)rtguest::G((uint32_t)(0x00100000)), (long)(*(int32_t*)rtguest::G(0x100004))));
    iVar4 = (*(int32_t*)rtguest::G(0x100004));
    iVar9 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x003fbb84 + iVar9)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x177f310 + (uint32_t)(iVar10 * 2) * 4)) = (uint32_t)((*(int32_t*)rtguest::G((uint32_t)__dest)) + (int)__dest);
    puVar11 = (uint32_t)(iVar4 + 0x100000);
    cVar2 = (*(uint8_t*)rtguest::G((uint32_t)0x100003 + (uint32_t)(iVar4) * 1));
    puVar3 = puVar11;
    while (cVar2 != 'p') {
      if (cVar2 == '\x10') {
        uVar8 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x10)));
      }
      else {
        fatal(0x2ea490);
        uVar8 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x10)));
      }
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x10))) =
           uVar8 & 0xffffc000ffffffff |
           ((long)(int)(((uint32_t)(uVar8 >> 0x20) & 0x3fff) +
                       ((int)((((uint32_t)bVar1 * 0x10 + uVar5) * 4 + 0x243e) * 0x10000) >> 0x10)) &
           0x3fffU) << 0x20;
      cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 0x10 + 0x13)));
      puVar3 = puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 8 + 8;
    }
    uVar7 = sceDmaGetChan(1);
    sceDmaSync(uVar7,0,0);
    sendTexturePackets(puVar11);
    if ((uint32_t)(iVar9 + (int)__dest) == (uint32_t)0x0) {
      uVar6 = 1;
    }
    else {
      uVar6 = 0;
    }
  }
  else {
    if ((int)(*(uint32_t*)rtguest::G(0x3356c0)) < 2) {
      if ((*(uint32_t*)rtguest::G(0x3356c0)) != 0) {
        return 0;
      }
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) = (char)((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) >> 8);
      cdReadEntry(((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) & 0xff) * 0xc + 0x29b428,0x100000);
      (*(uint32_t*)rtguest::G(0x3356c0)) = (*(uint32_t*)rtguest::G(0x3356c0)) | 0x8000;
    }
    else {
      if ((*(uint32_t*)rtguest::G(0x3356c0)) != 2) {
        return 0;
      }
      (*(uint32_t*)rtguest::G(0x3356c0)) = 0xffffffff;
    }
    uVar6 = 0;
  }
  return uVar6;
}


RT_GUEST_FN(0x0020c9d0, guest::loadCarModelAt)

}  // namespace guest
