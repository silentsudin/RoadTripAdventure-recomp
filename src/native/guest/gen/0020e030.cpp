// guest:0x0020e030 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SendTexturePacketsDMA(uint32_t param_1, uint32_t param_2, ...) {
  char cVar1;
uint32_t puVar2;
  undefined8 uVar3;
  ulong uVar4;
  
  cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 3)));
  puVar2 = param_1;
  while (cVar1 != 'p') {
    if (cVar1 == '\x10') {
      uVar4 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar2 + 0x10)));
    }
    else {
      fatal(0x2ea490);
      uVar4 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar2 + 0x10)));
    }
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar2 + 0x10))) =
         uVar4 & 0xffffc000ffffffff |
         (long)(int)(((uint32_t)(uVar4 >> 0x20) & 0x3fff) + (int)param_2 & 0x3fff) << 0x20;
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar2 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar2)) * 0x10 + 0x13)));
    puVar2 = puVar2 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar2)) * 8 + 8;
  }
  uVar3 = sceDmaGetChan(1);
  sceDmaSync(uVar3,0,0);
  sendTexturePackets(param_1);
  return 0;
}


RT_GUEST_FN(0x0020e030, guest::SendTexturePacketsDMA)

}  // namespace guest
