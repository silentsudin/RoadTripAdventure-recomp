// guest:0x00211648 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SpawnTasksFromResidentTable(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
uint32_t pcVar1;
  char cVar2;
  byte bVar3;
uint32_t pcVar4;
uint32_t piVar5;
  int iVar6;
  
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 4))) != (uint32_t)0x0) && (1 < (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 1))))) {
    bVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 1)));
    iVar6 = 0;
    if (0 < (int)(bVar3 - 1)) {
      pcVar4 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 4)))));
      piVar5 = 0x0177ea5c;
      do {
        cVar2 = (*(int8_t*)rtguest::G((uint32_t)pcVar4));
        iVar6 = iVar6 + 1;
        pcVar1 = pcVar4 + 1;
        pcVar4 = pcVar4 + 2;
        (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x2c22c8 + cVar2 * 4))) + (*(int8_t*)rtguest::G((uint32_t)pcVar1)) * 0x10;
        bVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 1)));
        piVar5 = piVar5 + 1;
      } while (iVar6 < (int)(bVar3 - 1));
    }
    gs_tasks_spawn(param_1,0x20dc50,bVar3 - 1 | param_3 | 0x70300);
  }
  return 0;
}


RT_GUEST_FN(0x00211648, guest::SpawnTasksFromResidentTable)

}  // namespace guest
