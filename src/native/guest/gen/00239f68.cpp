// guest:0x00239f68 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ShowRankedMessage(uint32_t param_1, ...) {
  char cVar1;
  byte bVar2;
uint32_t pcVar3;
  int iVar4;
  undefined8 uVar5;
  undefined8 uVar6;
  long lVar7;
  int iVar8;
  
  iVar8 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 8))) != 0) {
    lVar7 = uiMenuRun((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))));
    if (lVar7 != 0) {
      PlayUiSound(0x1a);
      iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x18)));
      bVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c))) + 2)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))) = (uint32_t)bVar2;
      gs_tasks_kill(param_1);
      PlayBgmSlot6();
      return 0;
    }
    return 0;
  }
  pcVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c)));
  uVar5 = uiMenuCreate();
  uiMenuAddItem(uVar5,0x3338e0,0);
  if ((*(int8_t*)rtguest::G((uint32_t)pcVar3)) == '\0') {
    uiMenuAddItem(uVar5,0x3338e8,0x80);
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)pcVar3));
  }
  else {
    uiMenuAddItem(uVar5,0x3338f0,0x80);
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)pcVar3));
  }
  uVar6 = DispatchByIndex16(cVar1,(*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(1) * sizeof(int8_t))));
  uiMenuAddItem(uVar5,uVar6,0x80);
  switch((*(int8_t*)rtguest::G((uint32_t)pcVar3))) {
  case '\x01':
    uVar6 = 0x3338f8;
    break;
  case '\x02':
    uVar6 = 0x2ec650;
    break;
  case '\x03':
    uVar6 = 0x2ec660;
    break;
  case '\x04':
    uVar6 = 0x333900;
    break;
  case '\x05':
    uVar6 = 0x2ec670;
    break;
  case '\x06':
  default:
    uiMenuAddItem(uVar5,0x3338f0,0x80);
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(1) * sizeof(int8_t)));
    goto LAB_0023a0b0;
  case '\a':
    uVar6 = 0x2ec680;
  }
  uiMenuAddItem(uVar5,uVar6,0x80);
  cVar1 = (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(1) * sizeof(int8_t)));
LAB_0023a0b0:
  SetFlaggedSlotBit((*(int8_t*)rtguest::G((uint32_t)pcVar3)),cVar1,0);
  PlayBgmSlot3();
  PlayUiSound(0x302);
  return 0;
}


RT_GUEST_FN(0x00239f68, guest::ShowRankedMessage)

}  // namespace guest
