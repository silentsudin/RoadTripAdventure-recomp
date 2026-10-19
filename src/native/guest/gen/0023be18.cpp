// guest:0x0023be18 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t dialogueCmdTextBox(uint32_t param_1, ...) {
  char cVar1;
  byte bVar2;
  int iVar3;
  int iVar4;
  
  iVar4 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 8))) == 0) {
    gs_tasks_spawn(param_1,0x25bcf0,2);
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x24)));
  }
  else {
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x24)));
  }
  if (cVar1 != '\0') {
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x18)));
    bVar2 = (*(int8_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c)))));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = (uint32_t)bVar2;
    gs_tasks_kill(param_1);
    return 0;
  }
}


RT_GUEST_FN(0x0023be18, guest::dialogueCmdTextBox)

}  // namespace guest
