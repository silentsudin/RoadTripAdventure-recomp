// guest:0x00239260 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DisplayMenuMessage(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  byte bVar1;
  int iVar2;
  int iVar3;
  undefined8 uVar4;
  long lVar5;
  int iVar6;
  
  iVar6 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) == 0) {
    uVar4 = uiMenuCreate();
    uiMenuAddItem(uVar4,(*(uint32_t*)rtguest::G((uint32_t)(0x2a2af0u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 2) * 4))),0x10);
    uiMenuAddItem(uVar4,(*(uint32_t*)rtguest::G((uint32_t)(0x2a2af4u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 2) * 4))),0x10);
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar4 + 0x5f))) = 2;
    CenterRectangle(uVar4,5);
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar4 + 0xe))) = 1;
  }
  else {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)));
    lVar5 = uiMenuRun(iVar2);
    if (lVar5 != 0) {
      if (lVar5 == -1) {
        (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xd))) = 1;
      }
      if (lVar5 == -2) {
        gs_tasks_kill(param_1);
        return 0;
      }
      PlayUiSound(0x1a);
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18)));
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x1c))) + (int)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xd))))));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = (uint32_t)bVar1;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xe))) = 2;
    }
  }
  return 0;
}


RT_GUEST_FN(0x00239260, guest::DisplayMenuMessage)

}  // namespace guest
