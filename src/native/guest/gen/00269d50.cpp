// guest:0x00269d50 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t shopNoBodiesLeftDialog(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  undefined8 uVar1;
  long lVar2;
  int iVar3;
uint32_t psVar4;
  
  iVar3 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) == 0) {
    uVar1 = uiMenuCreate();
    uiMenuAddItem(uVar1,0x32f3a0,0);
    uiMenuAddItem(uVar1,0x32f3b8,0);
    uiMenuAddItem(uVar1,0x335370,0x10);
    psVar4 = (uint32_t)uVar1;
    (*(int16_t*)rtguest::G((uint32_t)psVar4 + (uint32_t)(1) * sizeof(int16_t))) = 0x70 - (short)((int)((int)(*(int16_t*)rtguest::G((uint32_t)psVar4 + (uint32_t)(3) * sizeof(int16_t))) + (uint32_t)((uint32_t)(*(int16_t*)rtguest::G((uint32_t)psVar4 + (uint32_t)(3) * sizeof(int16_t))) >> 0xf)) >> 1);
    (*(int16_t*)rtguest::G((uint32_t)psVar4)) = 0x140 - (short)((int)((int)(*(int16_t*)rtguest::G((uint32_t)psVar4 + (uint32_t)(2) * sizeof(int16_t))) + (uint32_t)((uint32_t)(*(int16_t*)rtguest::G((uint32_t)psVar4 + (uint32_t)(2) * sizeof(int16_t))) >> 0xf)) >> 1);
  }
  else {
    lVar2 = uiMenuRun((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x28))));
    if (lVar2 != 0) {
      PlayUiSound(0x1a);
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = 0xffffffff;
      gs_tasks_kill(param_1);
      return 0;
    }
  }
  return 0;
}


RT_GUEST_FN(0x00269d50, guest::shopNoBodiesLeftDialog)

}  // namespace guest
