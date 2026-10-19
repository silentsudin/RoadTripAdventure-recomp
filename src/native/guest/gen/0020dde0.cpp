// guest:0x0020dde0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t loadFontTextures(uint32_t q0, ...) {
  char cVar1;
uint32_t puVar2;
  ulong uVar3;
  undefined8 uVar4;
  
  cdReadEntryWait(0x29af58,0x100000);
  puVar2 = 0x00100000;
  cVar1 = (*(int8_t*)rtguest::G(0x100003));
  while (cVar1 != 'p') {
    if (cVar1 == '\x10') {
      uVar3 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar2 + 0x10)));
    }
    else {
      fatal(0x2ea490);
      uVar3 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar2 + 0x10)));
    }
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar2 + 0x10))) =
         uVar3 & 0xffffc000ffffffff |
         ((long)(int)(((uint32_t)(uVar3 >> 0x20) & 0x3fff) + 0x1a40) & 0x3fffU) << 0x20;
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar2 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar2)) * 0x10 + 0x13)));
    puVar2 = puVar2 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar2)) * 8 + 8;
  }
  uVar4 = sceDmaGetChan(1);
  sceDmaSync(uVar4,0,0);
  sendTexturePackets(0x100000);
  return 0;
}


RT_GUEST_FN(0x0020dde0, guest::loadFontTextures)

}  // namespace guest
