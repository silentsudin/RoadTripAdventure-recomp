// guest:0x00211530 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SpawnTasksFromCarTable(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  char cVar1;
  int iVar2;
  long lVar3;
uint32_t piVar4;
  long lVar5;
  ulong uVar6;
  
  lVar5 = 0;
  iVar2 = (int)param_2 * 8;
  uVar6 = (uint32_t)(char)(*(uint32_t*)rtguest::G((uint32_t)0x2be43f + (uint32_t)(iVar2) * 4));
  cVar1 = (*(uint8_t*)rtguest::G((uint32_t)0x2be43e + (uint32_t)(iVar2) * 1));
  if (0 < (long)uVar6) {
    piVar4 = 0x0177ea60;
    do {
      iVar2 = (int)cVar1 + (int)lVar5;
      lVar5 = (long)((int)lVar5 + 1);
      lVar3 = MatchSlotPairState(param_2,iVar2);
      if (lVar3 == 0) {
        iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x2c22c8 + (int)param_2 * 4))) + iVar2 * 0x10;
      }
      else {
        iVar2 = (int)0x177f760 + (int)lVar3 * 0x1c;
      }
      (*(int32_t*)rtguest::G((uint32_t)piVar4)) = iVar2;
      piVar4 = piVar4 + 1;
    } while (lVar5 < (long)uVar6);
  }
  gs_tasks_spawn(param_1,0x20dc50,uVar6 | param_3 | 0x70400);
  return 0;
}


RT_GUEST_FN(0x00211530, guest::SpawnTasksFromCarTable)

}  // namespace guest
