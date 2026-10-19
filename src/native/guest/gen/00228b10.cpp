// guest:0x00228b10 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t LoadSaveSlotTask(uint32_t param_1, uint32_t param_2, ...) {
  byte bVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  
  iVar4 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 8))) == 0) {
    gs_tasks_spawn(param_1,0x2282c0,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c))));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25))) = 1;
  }
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x24))) == '\0') {
    return 0;
  }
  bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25)));  // pseudo byte
  if (bVar1 == 3) {
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))) = 0;
    gs_tasks_kill(param_1);
    return 0;
  }
  if (3 < bVar1) {
    if (bVar1 != 5) {
      if (bVar1 != 6) {
        return 0;
      }
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))) != 0) {
        PlayUiSound(0x1b);
        iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x18)));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x24))) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))) = 0;
        gs_tasks_kill(param_1);
        return 0;
      }
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x24))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c)));
      PlayUiSound(0x303);
      iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))) = 1;
      gs_tasks_kill(param_1);
      return 0;
    }
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))) != 0) {
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))) == 1) {
        gs_tasks_spawn(param_1,0x2280d8,0x100);
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25))) = 6;
        return 0;
      }
      uVar3 = 0x12;
      goto LAB_00228c80;
    }
LAB_00228c70:
    uVar3 = 0x1a;
LAB_00228c80:
    gs_tasks_spawn(param_1,0x227380,uVar3);
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25))) = 3;
    return 0;
  }
  if (bVar1 != 1) {
    return 0;
  }
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4)));
  if (iVar2 == 1) {
LAB_00228bc4:
    gs_tasks_spawn(param_1,0x228510,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c))));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25))) = 5;
  }
  else {
    if (iVar2 < 2) {
      if (iVar2 == 0) goto LAB_00228c70;
    }
    else if (iVar2 == 2) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x27))) = 1;
      goto LAB_00228bc4;
    }
    gs_tasks_spawn(param_1,0x227380,0x10);
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25))) = 3;
  }
  return 0;
}


RT_GUEST_FN(0x00228b10, guest::LoadSaveSlotTask)

}  // namespace guest
