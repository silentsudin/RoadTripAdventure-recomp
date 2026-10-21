// guest:0x00269d50 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t shopNoBodiesLeftDialog(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
  }
  undefined8 uVar1;
  long lVar2;
  int iVar3;
uint32_t psVar4;
  
  iVar3 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) == 0) {
    uVar1 = RT_CALLRA(0x269d6c, guest::uiMenuCreate(param_1) /* $a0 = entry $a0 at the jal (MIPS: no $4 write/jal before 0x269d6c); pseudocode dropped the live arg */);
    RT_CALLRA(0x269d84, guest::uiMenuAddItem(uVar1,0x32f3a0,0));
    RT_CALLRA(0x269d98, guest::uiMenuAddItem(uVar1,0x32f3b8,0));
    RT_CALLRA(0x269dac, guest::uiMenuAddItem(uVar1,0x335370,0x10));
    psVar4 = (uint32_t)uVar1;
    (*(int16_t*)rtguest::G((uint32_t)psVar4 + (uint32_t)(1) * sizeof(int16_t))) = 0x70 - (short)((int)((int)(*(int16_t*)rtguest::G((uint32_t)psVar4 + (uint32_t)(3) * sizeof(int16_t))) + (uint32_t)((uint32_t)(*(int16_t*)rtguest::G((uint32_t)psVar4 + (uint32_t)(3) * sizeof(int16_t))) >> 0xf)) >> 1);
    (*(int16_t*)rtguest::G((uint32_t)psVar4)) = 0x140 - (short)((int)((int)(*(int16_t*)rtguest::G((uint32_t)psVar4 + (uint32_t)(2) * sizeof(int16_t))) + (uint32_t)((uint32_t)(*(int16_t*)rtguest::G((uint32_t)psVar4 + (uint32_t)(2) * sizeof(int16_t))) >> 0xf)) >> 1);
  }
  else {
    lVar2 = (int32_t)RT_CALLRA(0x269e00, guest::uiMenuRun((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x28))),param_2));
    if (lVar2 != 0) {
      RT_CALLRA(0x269e10, guest::PlayUiSound(0x1a));
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = 0xffffffff;
      gs_tasks_kill(param_1);
      return 0;
    }
  }
  return 0;
}


RT_GUEST_FN(0x00269d50, *static_cast<uint32_t (*)(uint32_t, uint32_t, ...)>(&guest::shopNoBodiesLeftDialog))

}  // namespace guest
