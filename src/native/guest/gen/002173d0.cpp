// guest:0x002173d0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t PromptConfigConfirmationTask(uint32_t param_1, ...) {
  char cVar1;
  undefined1 uVar2;
  int iVar3;
  undefined8 uVar4;
  long lVar5;
uint32_t psVar6;
  int iVar7;
  
  iVar7 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8))) == 0) {
    uVar4 = uiMenuCreate();
    uiMenuAddItem(uVar4,0x333558,0);
    uiMenuAddItem(uVar4,0x333560,0x10);
    uiMenuAddItem(uVar4,0x333568,0x10);
    psVar6 = (uint32_t)uVar4;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar6 + 0xd))) = 1;
    uVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x1c)));
    (*(int16_t*)rtguest::G((uint32_t)psVar6 + (uint32_t)(1) * sizeof(int16_t))) = 0x9e;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(psVar6 + 6))) = uVar2;
    (*(int16_t*)rtguest::G((uint32_t)psVar6)) = 0x140 - (short)((int)((int)(*(int16_t*)rtguest::G((uint32_t)psVar6 + (uint32_t)(2) * sizeof(int16_t))) + (uint32_t)((uint32_t)(*(int16_t*)rtguest::G((uint32_t)psVar6 + (uint32_t)(2) * sizeof(int16_t))) >> 0xf)) >> 1);
  }
  else {
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28)));
    lVar5 = uiMenuRun(iVar3);
    if (lVar5 != 0) {
      if (lVar5 == -1) {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xd))) = 1;
        cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xd)));
      }
      else {
        cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xd)));
      }
      iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x24))) = 1;
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 4))) = (int)cVar1;
      gs_tasks_kill(param_1);
      return 0;
    }
  }
  return 0;
}


RT_GUEST_FN(0x002173d0, guest::PromptConfigConfirmationTask)

}  // namespace guest
