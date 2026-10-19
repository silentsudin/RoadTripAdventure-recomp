// guest:0x0026d438 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t InitSelectedSpawnPhase(uint32_t param_1, uint32_t param_2, ...) {
uint32_t puVar1;
  undefined8 uVar2;
  
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x14))) = 81000;
  InitProgressRamDefaults();
  uVar2 = __umoddi3((*(uint64_t*)rtguest::G((uint32_t)param_2)),6);
  switch(uVar2) {
  case 0:
    SelectProgressSlot(0x2b);
    break;
  case 1:
    SelectProgressSlot(0);
    break;
  case 2:
    SelectProgressSlot(0x75);
    break;
  case 3:
    SelectProgressSlot(0x8c);
    break;
  case 4:
    SelectProgressSlot(0x72);
    break;
  case 5:
    SelectProgressSlot(0x5d);
  }
  gs_tasks_spawn(param_1,0x20dc50,0x000a0000 + (*(uint8_t*)rtguest::G((uint32_t)0x0177f778 + 2)));
  puVar1 = (*(uint32_t*)rtguest::G(0x002c22e8));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xc))) = (*(uint8_t*)rtguest::G(0x26d550));
  (*(uint32_t*)rtguest::G(0x177ea50)) = puVar1 + 0x80;
  return 0;
}


RT_GUEST_FN(0x0026d438, guest::InitSelectedSpawnPhase)

}  // namespace guest
