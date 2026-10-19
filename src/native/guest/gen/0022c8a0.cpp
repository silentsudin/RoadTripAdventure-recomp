// guest:0x0022c8a0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RunSelectionMenuTask(uint32_t param_1, ...) {
  char cVar1;
  undefined1 uVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  long lVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  
  iVar8 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 8))) == 0) {
    uVar3 = uiMenuCreate();
    iVar7 = 0;
    do {
      lVar5 = GetFlaggedSlotCount(0xf,iVar7,0);
      iVar6 = iVar7 + 1;
      if (lVar5 != 0) {
        uVar4 = DispatchByIndex16(0xf,iVar7);
        uiMenuAddItem(uVar3,uVar4,0x10);
      }
      iVar7 = iVar6;
    } while (iVar6 < 10);
    uiMenuAddItem(uVar3,0x3336e8,0x10);
    iVar7 = (int)uVar3;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x5f))) = 2;
    CenterRectangle(uVar3,5);
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0xe))) = 1;
    uVar2 = CallCountFlaggedSlots(0xf,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c))),0);
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0xd))) = uVar2;
  }
  else {
    iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
    lVar5 = uiMenuRun(iVar7);
    if (lVar5 != 0) {
      if (lVar5 == -2) {
        gs_tasks_kill(param_1);
        return 0;
      }
      if (lVar5 != -1) {
        cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0xd)));
        if ((long)cVar1 != (long)(int)((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0xb))) - 1)) {
          iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x18)));
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x24))) = 1;
          (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 4))) = (int)cVar1;
          PlayUiSound(0x1a);
          gs_tasks_kill(param_1);
        }
      }
      PlayUiSound(0x1b);
      iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 4))) = 0xffffffff;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0xe))) = 2;
    }
  }
}


RT_GUEST_FN(0x0022c8a0, guest::RunSelectionMenuTask)

}  // namespace guest
