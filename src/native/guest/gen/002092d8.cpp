// guest:0x002092d8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gs_options_options_task(uint32_t param_1, ...) {
  char cVar1;
  byte bVar2;
  int iVar3;
  long lVar4;
  undefined8 uVar5;
  int iVar6;
  
  iVar6 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) == 0) {
    iVar3 = uiMenuCreate();
    uiMenuAddItem(iVar3,(*(uint32_t*)rtguest::G((uint32_t)(0x29ac90u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 5) * 4))),0x10);
    uiMenuAddItem(iVar3,(*(uint32_t*)rtguest::G((uint32_t)(0x29ac94u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 5) * 4))),0x10);
    uiMenuAddItem(iVar3,(*(uint32_t*)rtguest::G((uint32_t)(0x29ac98u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 5) * 4))),0x10);
    uiMenuAddItem(iVar3,(*(uint32_t*)rtguest::G((uint32_t)(0x29aca0u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 5) * 4))),0x10);
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xe))) = 1;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xc))) = 2;
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = 0xb4;
  }
  else {
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)));
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) != '\0') {
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xe))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xe))) & 0x7f;
    }
    lVar4 = uiMenuRun(iVar3);
    if (lVar4 != 0) {
      if (lVar4 == -1) {
        (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xd))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xb))) + -1;
      }
      if (lVar4 == -2) {
        gs_tasks_kill(param_1);
        bVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xe)));
        goto LAB_002094d4;
      }
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xd)));
      if (cVar1 == '\x01') {
        PlayUiSound(0x1a);
        uVar5 = 0x208f50;
      }
      else if (cVar1 < '\x02') {
        if (cVar1 != '\0') {
          bVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xe)));
          goto LAB_002094d4;
        }
        PlayUiSound(0x1a);
        uVar5 = 0x208c98;
      }
      else {
        if (cVar1 != '\x02') {
          if (cVar1 != '\x03') {
            bVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xe)));
            goto LAB_002094d4;
          }
          PlayUiSound(0x1b);
          iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18)));
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) = 1;
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) = 1;
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xe))) = 2;
          goto LAB_002094d0;
        }
        PlayUiSound(0x1a);
        uVar5 = 0x2090c0;
      }
      gs_tasks_spawn(param_1,uVar5,0);
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xe))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xe))) | 0x80;
    }
  }
LAB_002094d0:
  bVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xe)));
LAB_002094d4:
  if ((bVar2 & 0x80) != 0) {
    uiDrawHintBar(3);
    return 0;
  }
  uiDrawHintBar(2);
  return 0;
}


RT_GUEST_FN(0x002092d8, guest::gs_options_options_task)

}  // namespace guest
