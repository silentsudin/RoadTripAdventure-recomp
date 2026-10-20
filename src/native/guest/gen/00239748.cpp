// guest:0x00239748 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ShowSequencedMessages(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  byte bVar1;
  int iVar2;
  int iVar3;
  char cVar4;
  undefined8 uVar5;
  undefined8 uVar6;
  long lVar7;
  int iVar8;
  
  iVar8 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 8))) == 0) {
    uVar5 = uiMenuCreate();
    cVar4 = CheckSlotPresence();
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))) = cVar4;
    if (cVar4 != '\0') {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 8))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0xc))) = 0x239378;
      ShowAwardMessage(param_1,param_2);
      return 0;
    }
    uVar6 = GetSlotDataPtr(1,0);
    uiMenuAddItem(uVar5,uVar6,0x10);
    uVar6 = GetSlotDataPtr(2,0);
    uiMenuAddItem(uVar5,uVar6,0x10);
    uiMenuAddItem(uVar5,0x3338a8,0x10);
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar5 + 0x5f))) = 2;
    CenterRectangle(uVar5,5);
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar5 + 0xe))) = 1;
  }
  else {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
    lVar7 = uiMenuRun(iVar2);
    if (lVar7 != 0) {
      if (lVar7 == -1) {
        (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xd))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xb))) + -1;
      }
      if (lVar7 == -2) {
        gs_tasks_kill(param_1);
        return 0;
      }
      PlayUiSound(0x1a);
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xd))) != '\x02') {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0xc))) = 0x239508;
        cVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xd)));
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 8))) = 0;
        (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))) = cVar4 + '\x01';
        ShowResidentMessage(param_1,param_2);
        return 0;
      }
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x18)));
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c))) + 1)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = (uint32_t)bVar1;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xe))) = 2;
    }
  }
  return 0;
}


RT_GUEST_FN(0x00239748, guest::ShowSequencedMessages)

}  // namespace guest
