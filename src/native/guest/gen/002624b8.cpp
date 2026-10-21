// guest:0x002624b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t noMoneyMenuTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
  }
  char cVar1;
  int iVar2;
  undefined8 uVar3;
  long lVar4;
uint32_t psVar5;
  int iVar6;
  
  iVar6 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) == 0) {
    uVar3 = RT_CALLRA(0x2624d8, guest::uiMenuCreate(param_1));
    RT_CALLRA(0x2624f0, guest::uiMenuAddItem(uVar3,0x3292b0,0));
    RT_CALLRA(0x262504, guest::uiMenuAddItem(uVar3,0x334e98,0x10));
    psVar5 = (uint32_t)uVar3;
    (*(int16_t*)rtguest::G((uint32_t)psVar5 + (uint32_t)(1) * sizeof(int16_t))) = 0x9e;
    (*(int16_t*)rtguest::G((uint32_t)psVar5)) = 0x140 - (short)((int)((int)(*(int16_t*)rtguest::G((uint32_t)psVar5 + (uint32_t)(2) * sizeof(int16_t))) + (uint32_t)((uint32_t)(*(int16_t*)rtguest::G((uint32_t)psVar5 + (uint32_t)(2) * sizeof(int16_t))) >> 0xf)) >> 1);
  }
  else {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)));
    lVar4 = (int32_t)RT_CALLRA(0x262540, guest::uiMenuRun(iVar2,param_2));
    if (lVar4 != 0) {
      RT_CALLRA(0x262550, guest::PlayUiSound(0x1a));
      iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18)));
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xd)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) = 1;
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) = (int)cVar1;
      gs_tasks_kill(param_1);
      return 0;
    }
  }
  return 0;
}


RT_GUEST_FN(0x002624b8, *static_cast<uint32_t (*)(uint32_t, uint32_t, ...)>(&guest::noMoneyMenuTask))

}  // namespace guest
