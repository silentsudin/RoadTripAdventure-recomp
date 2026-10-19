// guest:0x0026b470 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t grandPrixMenuScene(uint32_t param_1, uint32_t param_2, ...) {
  byte bVar1;
  int iVar2;
  undefined8 uVar3;
  long lVar4;
uint32_t pbVar5;
  int iVar6;
  
  iVar6 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) == 0) {
    iVar6 = 0x1c;
    uVar3 = uiMenuCreate(param_1);
    pbVar5 = 0x002bdf90;
    bVar1 = (*(uint8_t*)rtguest::G(0x2bdf90));
    while( true ) {
      pbVar5 = pbVar5 + 0x10;
      iVar6 = iVar6 + 1;
      uiMenuAddItem(uVar3,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x2be398 + (uint32_t)(bVar1) * 4)),0x10);
      if (0x22 < iVar6) break;
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar5));
    }
  }
  else if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) == '\0') {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x50))) = 0x8033cc66;
    lVar4 = uiMenuRun(iVar2,param_2);
    if (lVar4 == 1) {
      PlayUiSound(0x1a);
      gs_tasks_spawn(param_1,0x20f950,(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xd))) + 0x1c);
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) = 1;
    }
    else if (lVar4 == -1) {
      PlayUiSound(0x1b);
      iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18)));
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) = 1;
      gs_tasks_kill(param_1);
      return 0;
    }
  }
  else if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) != '\0') {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) = 0;
  }
  return 0;
}


RT_GUEST_FN(0x0026b470, guest::grandPrixMenuScene)

}  // namespace guest
