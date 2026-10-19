// guest:0x00204de8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gs_tasks_kill(uint32_t param_1, ...) {
uint32_t puVar1;
  int iVar2;
uint32_t pcVar3;
uint32_t pcVar4;
  
  if ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x2c))) == (uint32_t)0x0) {
    puVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10)));
  }
  else {
    rtCall((*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x2c))))), param_1); // arg-drop fix: $4 still holds param_1 at the jalr
    puVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10)));
  }
  (*(uint8_t*)rtguest::G((uint32_t)param_1)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) = 0;
  if (puVar1 != (uint32_t)0x0) {
    if ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x2c))) == (uint32_t)0x0) {
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x10)));
    }
    else {
      rtCall((*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x2c))))), puVar1);
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x10)));
    }
    (*(uint8_t*)rtguest::G((uint32_t)puVar1)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x28))) = 0;
    if (iVar2 != 0) {
      DestroyTaskTree(iVar2); // arg-drop fix: $4=iVar2 at the jal
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x10))) = 0;
    }
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x14))) == 0) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10))) = 0;
    }
    else {
      DestroyTaskTree((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x14)))); // arg-drop fix: $4=*(puVar1+0x14) at the jal
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x14))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10))) = 0;
    }
  }
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x18))) != 0) {
    pcVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x18))) + 0x10)));
    pcVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(pcVar4 + 0x14)));
    if ((*(int8_t*)rtguest::G((uint32_t)pcVar4)) == '\0') {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(pcVar4 + 0x18))) + 0x10))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(pcVar4 + 0x14)));
    }
    else {
      for (; (*(int8_t*)rtguest::G((uint32_t)pcVar3)) != '\0'; pcVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(pcVar3 + 0x14)))) {
        pcVar4 = pcVar3;
      }
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(pcVar4 + 0x14))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(pcVar3 + 0x14)));
    }
  }
  return 0;
}


RT_GUEST_FN(0x00204de8, guest::gs_tasks_kill)

}  // namespace guest
