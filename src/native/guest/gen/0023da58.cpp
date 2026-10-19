// guest:0x0023da58 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t InitActorSlot(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
uint32_t puVar1;
  
  puVar1 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x177ea50 + (uint32_t)((int)param_1) * 4));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0177f778 + param_2 * 0x1c + 4))) = (*(uint32_t*)rtguest::G((uint32_t)puVar1));
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0177f780 + param_2 * 0x1c))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 1)));
  copyCarModel(param_1,param_2 + 1);
  (*(int8_t*)rtguest::G((uint32_t)0x177fdd4 + (uint32_t)(param_2 * 2) * 1)) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x23)));
  (*(uint32_t*)rtguest::G((uint32_t)0x177fdd5 + (uint32_t)(param_2 * 2) * 4)) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x21)));
  CopyAiRouteTables(param_2 * 0x1000 + 0x1780a60);
  return 0;
}


RT_GUEST_FN(0x0023da58, guest::InitActorSlot)

}  // namespace guest
