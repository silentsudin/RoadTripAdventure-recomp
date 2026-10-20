// guest:0x00239150 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DisplayTextDialog(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
  }
  char cVar1;
  int iVar2;
  undefined8 uVar3;
  long lVar4;
  int iVar5;
  
  iVar5 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) == 0) {
    uVar3 = RT_CALLRA(0x239174, guest::uiMenuCreate());
    RT_CALLRA(0x23918c, guest::uiMenuAddItem(uVar3,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c))),0));
    RT_CALLRA(0x2391ac, guest::uiMenuAddItem(uVar3,(*(uint32_t*)rtguest::G((uint32_t)(0x2a2af0u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 2) * 4))),0x10));
    RT_CALLRA(0x2391c8, guest::uiMenuAddItem(uVar3,(*(uint32_t*)rtguest::G((uint32_t)(0x2a2af4u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 2) * 4))),0x10));
    if (((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10))) & 2) != 0) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar3 + 0xc))) = 2;
    }
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar3 + 0xd))) = 1;
  }
  else {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28)));
    lVar4 = RT_CALLRA(0x2391f4, guest::uiMenuRun(iVar2));
    if (lVar4 != 0) {
      if (lVar4 == -1) {
        (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xd))) = 1;
        cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xd)));
      }
      else {
        cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xd)));
      }
      iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24))) = 1;
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) = (int)cVar1;
      gs_tasks_kill(param_1);
      return 0;
    }
  }
  return 0;
}


RT_GUEST_FN(0x00239150, guest::DisplayTextDialog)

}  // namespace guest
