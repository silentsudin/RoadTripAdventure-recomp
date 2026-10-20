// guest:0x0023bcd0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t dialogueCmdTabListMenu(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
  byte bVar1;
  int iVar2;
  int iVar3;
  undefined8 uVar4;
  long lVar5;
uint32_t pcVar6;
uint32_t pcVar7;
  int iVar8;
  
  iVar8 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 8))) == 0) {
    pcVar7 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c)));
    uVar4 = RT_CALLRA(0x23bcf8, guest::uiMenuCreate(param_1));
    do {
      RT_CALLRA(0x23bd14, guest::uiMenuAddItem(uVar4,pcVar7,0x10));
      do {
        pcVar6 = pcVar7;
        pcVar7 = pcVar6 + 1;
      } while ((*(int8_t*)rtguest::G((uint32_t)pcVar6)) != '\0');
    } while ((*(int8_t*)rtguest::G((uint32_t)pcVar7)) != '\t');
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x20))) = pcVar6 + 2;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar4 + 0x5f))) = 2;
    RT_CALLRA(0x23bd58, guest::CenterRectangle(uVar4,5));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar4 + 0xe))) = 1;
  }
  else {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
    lVar5 = RT_CALLRA(0x23bd70, guest::uiMenuRun(iVar2));
    if (lVar5 != 0) {
      if (lVar5 == -1) {
        (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xd))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xb))) + -1;
      }
      if (lVar5 == -2) {
        gs_tasks_kill(param_1);
        return 0;
      }
      RT_CALLRA(0x23bda4, guest::PlayUiSound(0x1a));
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x18)));
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x20))) + (int)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xd))))));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = (uint32_t)bVar1;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xe))) = 2;
    }
  }
  return 0;
}


RT_GUEST_FN(0x0023bcd0, guest::dialogueCmdTabListMenu)

}  // namespace guest
