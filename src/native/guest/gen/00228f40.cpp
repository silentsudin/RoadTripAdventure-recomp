// guest:0x00228f40 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t LoadOrCreateCardSlotTask(uint32_t param_1, ...) {
  char cVar1;
  undefined1 uVar2;
  int iVar3;
  int iVar4;
  
  iVar4 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 8))) == 0) {
    gs_tasks_spawn(param_1,0x2282c0,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c))));
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x24)));
  }
  else {
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x24)));
  }
  if (cVar1 == '\0') {
    return 0;
  }
  switch((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25)))) {
  case 0:
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4)));
    if (iVar3 == 0) {
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c))) != 0) {
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c))) + 4;
        break;
      }
      gs_tasks_spawn(param_1,0x2282c0,1);
      uVar2 = 4;
    }
    else {
      if (iVar3 < 0) {
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c)));
      }
      else {
        if (iVar3 < 3) {
          gs_tasks_spawn(param_1,0x228510,(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c))) * 0x101);
          uVar2 = 1;
          goto LAB_002291b0;
        }
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c)));
      }
      if (iVar3 != 0) break;
      gs_tasks_spawn(param_1,0x2282c0,1);
      uVar2 = 3;
    }
  case 1:
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))) == 0) {
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c))) == 0) {
        gs_tasks_spawn(param_1,0x2282c0,1);
        uVar2 = 4;
      }
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c))) + 4;
    }
    else {
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))) == 1) {
        iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x18)));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x24))) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))) = 1;
        gs_tasks_kill(param_1);
      }
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c))) + 8;
    }
    break;
  case 2:
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))) = 0;
    gs_tasks_kill(param_1);
  case 3:
    if (((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))) < 3) && (-1 < (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))))) {
      iVar3 = 0;
    }
    else {
      iVar3 = 6;
    }
    break;
  case 4:
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4)));
    if (iVar3 == 0) {
      iVar3 = 7;
    }
    else {
      if ((-1 < iVar3) && (iVar3 < 3)) {
        gs_tasks_spawn(param_1,0x228510,0x101);
        uVar2 = 5;
      }
LAB_0022919c:
      iVar3 = 4;
    }
    break;
  case 5:
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))) != 0) goto LAB_0022919c;
    iVar3 = 7;
    break;
  default:
    goto switchD_00228f98_default;
  }
  gs_tasks_spawn(param_1,0x227380,iVar3);
  uVar2 = 2;
LAB_002291b0:
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25))) = uVar2;
switchD_00228f98_default:
}


RT_GUEST_FN(0x00228f40, guest::LoadOrCreateCardSlotTask)

}  // namespace guest
