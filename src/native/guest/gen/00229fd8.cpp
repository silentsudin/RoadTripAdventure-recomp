// guest:0x00229fd8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t InitProgressRamDefaults(uint32_t q0, ...) {
uint32_t puVar1;
  int iVar2;
uint32_t ppuVar3;
uint32_t puVar4;
  
  InitSaveSlotDefaults(0);
  InitSaveSlotDefaults(1);
  (*(uint32_t*)rtguest::G(0x177fdb4)) = 1000;
  iVar2 = 0x96;
  puVar4 = 0x017804d0;
  ppuVar3 = 0x2ed724u;
  do {
    puVar1 = (*(uint32_t*)rtguest::G((uint32_t)ppuVar3));
    ppuVar3 = ppuVar3 + 0x44;
    iVar2 = iVar2 + -1;
    (*(uint32_t*)rtguest::G((uint32_t)puVar4)) = puVar1;
    puVar4 = puVar4 + sizeof(uint32_t);
  } while (-1 < iVar2);
  (*(uint32_t*)rtguest::G((uint32_t)0x177f760 + 0)) = 0x00f30f30;
  return 0;
}


RT_GUEST_FN(0x00229fd8, guest::InitProgressRamDefaults)

}  // namespace guest
