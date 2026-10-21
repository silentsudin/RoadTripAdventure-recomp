// guest:0x0026bc08 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t pauseMenuTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
  }
  int iVar1;
  undefined8 uVar2;
  long lVar3;
  int iVar4;
  
  iVar4 = (int)param_1;
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x28)));
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 8))) == 0) {
    uVar2 = RT_CALLRA(0x26bc30, guest::uiMenuCreate(param_1));  // $4 untouched 26bc18->26bc30
    RT_CALLRA(0x26bc48, guest::uiMenuAddItem(uVar2,0x32f9f0,0x10));
    RT_CALLRA(0x26bc5c, guest::uiMenuAddItem(uVar2,0x335608,0x10));
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10))) == '\0') {
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x22))) < '\x12') {
        RT_CALLRA(0x26bc8c, guest::uiMenuAddItem(uVar2,0x335610,0x10));
        iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c)));
      }
      else {
        iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c)));
      }
    }
    else {
      iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c)));
    }
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar2 + 0xe))) = 3;
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar2 + 0xc))) = '\t' - (iVar1 == 0);
  }
  else {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0xe))) = 0;
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25))) == '\0') {
      lVar3 = (int32_t)RT_CALLRA(0x26bce0, guest::uiMenuRun(iVar1,param_2));
      if (lVar3 != 0) {
        if (lVar3 == -1) {
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0xd))) = 0;
        }
        switch((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0xd)))) {
        case 0:
          iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x18)));
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x24))) = 1;
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 4))) = 0xffffffff;
          RT_CALLRA(0x26bd3c, guest::gs_tasks_kill(param_1));
          break;
        case 1:
          iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x18)));
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x50))) = 0x80000000;
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 0x400000;
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x24))) = 1;
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 4))) = 0;
          RT_CALLRA(0x26bd78, guest::gs_tasks_kill(param_1));
          break;
        case 2:
          if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10))) == '\0') && ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x22))) < '\x12')) {
            RT_CALLRA(0x26bdb0, guest::gs_tasks_spawn(param_1,0x237878,0));
            (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25))) = 1;
          }
          break;
        case 3:
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) ^ 0x10000;
          RT_CALLRA(0x26bdd8, guest::PlayUiSound(0x1a));
          break;
        case 4:
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x54))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x54))) | 1;
          (*(uint32_t*)rtguest::G(0x177ae8c)) = 0x40000;
          RT_CALLRA(0x26be00, guest::PlayUiSound(0x1a));
          break;
        case 5:
          (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) ^ 0x80;
          break;
        case 6:
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xc))) = 0x26d020;
        }
      }
    }
    else if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x24))) != '\0') {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25))) = 0;
    }
  }
  uiFade(0x40000000);
  return 0;
}


RT_GUEST_FN(0x0026bc08, guest::pauseMenuTask)

}  // namespace guest
