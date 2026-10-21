// guest:0x0023bb00 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ShowMessageAndWait(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc) = 0x00000000u;
  }
  undefined8 uVar1;
  long lVar2;
  int iVar3;
  
  iVar3 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) == 0) {
    uVar1 = RT_CALLRA(0x23bb1c, guest::uiMenuCreate(param_1) /* $a0 = entry $a0 at the jal (MIPS: no $4 write/jal before 0x23bb1c); pseudocode dropped the live arg */);
    RT_CALLRA(0x23bb2c, guest::uiMenuAddItem(uVar1,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x1c))),0));
    PlayUiSound(0x1e);
    return 0;
  }
  lVar2 = (int32_t)RT_CALLRA(0x23bb48, guest::uiMenuRun((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x28))),param_2));
  if (lVar2 != 0) {
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = 0;
    gs_tasks_kill(param_1);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x0023bb00, *static_cast<uint32_t (*)(uint32_t, uint32_t, ...)>(&guest::ShowMessageAndWait))

}  // namespace guest
