// guest:0x00261508 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t confirmYesNoMenuTask(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
  int iVar1;
  char cVar2;
  undefined8 uVar3;
  long lVar4;
uint32_t psVar5;
  int iVar6;
  
  iVar6 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) == 0) {
    uVar3 = RT_CALLRA(0x261528, guest::uiMenuCreate(param_1));
    RT_CALLRA(0x261540, guest::uiMenuAddItem(uVar3,0x334ea0,0));
    RT_CALLRA(0x261554, guest::uiMenuAddItem(uVar3,0x334e98,0x10));
    RT_CALLRA(0x261568, guest::uiMenuAddItem(uVar3,0x334ea8,0x10));
    psVar5 = (uint32_t)uVar3;
    (*(int16_t*)rtguest::G((uint32_t)psVar5 + (uint32_t)(1) * sizeof(int16_t))) = 0x77;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar5 + 0xd))) = 0;
    (*(int16_t*)rtguest::G((uint32_t)psVar5)) = 0x140 - (short)((int)((int)(*(int16_t*)rtguest::G((uint32_t)psVar5 + (uint32_t)(2) * sizeof(int16_t))) + (uint32_t)((uint32_t)(*(int16_t*)rtguest::G((uint32_t)psVar5 + (uint32_t)(2) * sizeof(int16_t))) >> 0xf)) >> 1);
  }
  else {
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)));
    lVar4 = RT_CALLRA(0x2615a8, guest::uiMenuRun(iVar1));
    if (lVar4 != 0) {
      if (lVar4 == -1) {
        (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0xd))) = 1;
        cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0xd)));
      }
      else {
        cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0xd)));
      }
      if (cVar2 == '\0') {
        RT_CALLRA(0x2615d4, guest::PlayUiSound(0x1a));
        cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0xd)));
      }
      else {
        RT_CALLRA(0x2615e8, guest::PlayUiSound(0x1b));
        cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0xd)));
      }
      iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) = 1;
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) = (int)cVar2;
      gs_tasks_kill(param_1);
      return 0;
    }
  }
  return 0;
}


RT_GUEST_FN(0x00261508, guest::confirmYesNoMenuTask)

}  // namespace guest
