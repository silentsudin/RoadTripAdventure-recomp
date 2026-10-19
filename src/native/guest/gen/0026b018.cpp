// guest:0x0026b018 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t AdjusterMenuTask(uint32_t param_1, uint32_t param_2, ...) {
  char cVar1;
uint32_t puVar2;
  undefined8 uVar3;
  long lVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  
  iVar6 = (int)param_1;
  iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)));
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) == 0) {
    uVar3 = uiMenuCreate(param_1);
    uiMenuAddItem(uVar3,0x32f8d0,0x10);
    uiMenuAddItem(uVar3,0x32f8e0,0x10);
    uiMenuAddItem(uVar3,0x32f8f0,0x10);
    iVar5 = (int)uVar3;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xc))) = 1;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xe))) = 3;
    if ((*(int32_t*)rtguest::G(0x177ae8c)) == 0x40000) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x51))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x51))) | 1;
    }
  }
  else {
    iVar7 = (int)param_2;
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) & 0x10000) == 0) {
      puVar2 = 0x0032f8f0;
    }
    else {
      puVar2 = 0x0032f908;
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c))) = puVar2;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xe))) = 0;
    lVar4 = uiMenuRun(iVar5,param_2);
    if (lVar4 != 0) {
      if (lVar4 == -1) {
        (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xd))) = 0;
      }
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xd)));
      if (cVar1 == '\x01') {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x51))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x51))) | 1;
        (*(int32_t*)rtguest::G(0x177ae8c)) = 0x40000;
      }
      else if (cVar1 < '\x02') {
        if (cVar1 == '\0') {
          iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18)));
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24))) = 1;
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) = 0xffffffff;
          gs_tasks_kill(param_1);
        }
      }
      else if (cVar1 == '\x02') {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) ^ 0x10000;
        PlayUiSound(0x1a);
      }
    }
  }
  uiFade(0xffffffff80006666);
  return 0;
}


RT_GUEST_FN(0x0026b018, guest::AdjusterMenuTask)

}  // namespace guest
