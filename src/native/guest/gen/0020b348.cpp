// guest:0x0020b348 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t loadPoseSet(uint32_t param_1, ...) {
  char cVar1;
  ushort uVar2;
uint32_t puVar3;
  ulong uVar4;
  undefined8 uVar5;
uint32_t ppuVar6;
uint32_t puVar7;
uint32_t puVar8;
  
  switch((*(uint32_t*)rtguest::G(0x3356c0))) {
  case 0:
    ppuVar6 = 0x29afc4u;
    break;
  case 1:
    puVar3 = 0x00100000;
    cVar1 = (*(uint8_t*)rtguest::G(0x100003));
    while (cVar1 != 'p') {
      if (cVar1 == '\x10') {
        uVar4 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x10)));
      }
      else {
        fatal(0x2ea490);
        uVar4 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x10)));
      }
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x10))) =
           uVar4 & 0xffffc000ffffffff |
           ((long)(int)(((uint32_t)(uVar4 >> 0x20) & 0x3fff) + 0x1d8e) & 0x3fffU) << 0x20;
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 0x10 + 0x13)));
      puVar3 = puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 8 + 8;
    }
    uVar5 = sceDmaGetChan(1);
    sceDmaSync(uVar5,0,0);
    sendTexturePackets(0x100000);
    return 0;
  case 2:
    ppuVar6 = 0x29b00cu;
    break;
  case 3:
    (uint32_t)(uintptr_t)(::memmove((char *)rtguest::G((uint32_t)(0x003fa780)), (char *)rtguest::G((uint32_t)(0x00100000)), (long)(*(uint32_t*)rtguest::G(0x100004))));
    (*(uint32_t*)rtguest::G(0x335718)) = 0x003fa780 + (*(uint32_t*)rtguest::G(0x3fa780));
    puVar7 = (uint32_t)((*(uint32_t*)rtguest::G(0x100004)) + 0x100000);
    puVar8 = 0x003fa780 + (*(uint32_t*)rtguest::G(0x3fa784));
    cVar1 = (*(uint8_t*)rtguest::G((uint32_t)0x100003 + (uint32_t)((*(uint32_t*)rtguest::G(0x100004))) * 1));
    puVar3 = puVar7;
    while (cVar1 != 'p') {
      if (cVar1 == '\x10') {
        uVar2 = (*(uint16_t*)rtguest::G((uint32_t)puVar3));
      }
      else {
        fatal(0x2ea490);
        uVar2 = (*(uint16_t*)rtguest::G((uint32_t)puVar3));
      }
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x10))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x10)));
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + (uint32_t)uVar2 * 0x10 + 0x13)));
      puVar3 = puVar3 + (uint32_t)uVar2 * 8 + 8;
    }
    uVar5 = sceDmaGetChan(1);
    sceDmaSync(uVar5,0,0);
    sendTexturePackets(puVar7);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20))) = puVar8;
    if (puVar8 == (uint32_t)0x0) {
      return 1;
    }
    if ((int)puVar8 < 0x3fbb81) {
    }
    fatal(0x2ea4c0);
  case 4:
    (*(uint32_t*)rtguest::G(0x3356c0)) = 0xffffffff;
  default:
    goto switchD_0020b390_default;
  }
  cdReadEntry(ppuVar6,0x100000);
  (*(uint32_t*)rtguest::G(0x3356c0)) = (*(uint32_t*)rtguest::G(0x3356c0)) | 0x8000;
switchD_0020b390_default:
}


RT_GUEST_FN(0x0020b348, guest::loadPoseSet)

}  // namespace guest
