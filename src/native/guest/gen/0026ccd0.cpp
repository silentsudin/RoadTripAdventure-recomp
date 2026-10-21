// guest:0x0026ccd0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DispatchAreaEntryTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
  }
  char cVar1;
  int iVar2;
  undefined8 uVar3;
  long lVar4;
uint32_t pcVar5;
  int iVar6;
  
  iVar6 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) == 0) {
    uVar3 = RT_CALLRA(0x26ccf4, guest::uiMenuCreate(param_1) /* $a0 = entry $a0 at the jal (MIPS: no $4 write/jal before 0x26ccf4); pseudocode dropped the live arg */);
    uiMenuAddItem(uVar3,0x32fa30,0x10);
    uiMenuAddItem(uVar3,0x32fa40,0x10);
    uiMenuAddItem(uVar3,0x32fa50,0x10);
    uiMenuAddItem(uVar3,0x32fa60,0x10);
    uiMenuAddItem(uVar3,0x32fa70,0x10);
    uiMenuAddItem(uVar3,0x32fa80,0x10);
    uiMenuAddItem(uVar3,0x32fa90,0x10);
    uiMenuAddItem(uVar3,0x32faa0,0x10);
    uiMenuAddItem(uVar3,0x32fab0,0x10);
    uiMenuAddItem(uVar3,0x32fac0,0x10);
    uiMenuAddItem(uVar3,0x32fad8,0x10);
    uiMenuAddItem(uVar3,0x32faf0,0x10);
    return 0;
  }
  if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x30))) & 0x800) == 0) {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) != '\0') {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) = 0;
    }
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25)));
  }
  else {
    RT_CALLRA(0x26ce18, guest::DestroyTaskRecord());
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) = 0;
    RT_CALLRA(0x26ce20, guest::SoundStopAllChannels());
    RT_CALLRA(0x26ce28, guest::PlayBgmSlot3());
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25)));
  }
  if (cVar1 == '\0') {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x50))) = 0x8033cc66;
    lVar4 = (int32_t)RT_CALLRA(0x26ce64, guest::uiMenuRun(iVar2,param_2));
    if (lVar4 == 1) {
      RT_CALLRA(0x26ce78, guest::PlayUiSound(0x1a));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) = 1;
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xd)));
      if (cVar1 == '\n') {
        pcVar5 = 0x26cb60;
      }
      else {
        if (cVar1 != '\v') {
          RT_CALLRA(0x26cebc, guest::gs_tasks_spawn(param_1,0x26ca48));
          return 0;
        }
        pcVar5 = 0x26d2c0;
      }
      RT_CALLRA(0x26ced4, guest::gs_tasks_spawn(param_1,pcVar5,0));
    }
    else if (lVar4 == -1) {
      iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) = 0;
      RT_CALLRA(0x26cefc, guest::gs_tasks_kill(param_1));
    }
  }
  return 0;
}


RT_GUEST_FN(0x0026ccd0, guest::DispatchAreaEntryTask)

}  // namespace guest
