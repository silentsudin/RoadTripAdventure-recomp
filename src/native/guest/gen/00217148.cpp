// guest:0x00217148 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gs_options_pause_task(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
  }
  char cVar1;
  undefined8 uVar2;
  int iVar3;
  
  iVar3 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) == 0) {
    RT_CALLRA(0x217174, guest::InitPauseMenuOptions(param_1,param_2,0x1727b90));
  }
  if ((*(uint32_t*)rtguest::G(0x1727b90)) != (*(int32_t*)rtguest::G(0x1727b94))) {
    (*(uint32_t*)rtguest::G(0x1727b90)) = (*(int32_t*)rtguest::G(0x1727b94));
    (*(uint32_t*)rtguest::G(0x1727b98)) = 0;
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(0x0172fd34)), 0, 500));
  }
  switch((*(uint32_t*)rtguest::G(0x1727b90))) {
  case 0:
    RT_CALLRA(0x2171e0, guest::UpdatePauseMenuTask(param_1,param_2,0x1727b90,0x172fbb0));
    break;
  case 1:
    RT_CALLRA(0x217220, guest::UpdatePauseSubmenuTask(param_1,param_2,0x1727b90,0x172fc18));
    break;
  case 2:
    RT_CALLRA(0x217200, guest::UpdatePauseConfirmDialogTask(param_1,param_2,0x1727b90,0x172fc80));
    break;
  case 3:
    RT_CALLRA(0x217240, guest::UpdateAudioSettingsMenuTask(param_1,param_2,0x1727b90,0x172fca4));
    break;
  case 4:
    RT_CALLRA(0x217260, guest::UpdateGarageMenuTask(param_1,param_2,0x1727b90,0x172fcb8));
    break;
  case 5:
    RT_CALLRA(0x217280, guest::gs_options_settings_page(param_1,param_2,0x1727b90,0x172fcc0));
    break;
  case 6:
    RT_CALLRA(0x2172a0, guest::UpdateWorldMapMenuTask(param_1,param_2,0x1727b90,0x172fd2c));
    break;
  case 7:
    if ((*(uint32_t*)rtguest::G(0x1727b98)) == 0) {
      (*(uint32_t*)rtguest::G(0x172fc90)) = 0;
      (*(uint32_t*)rtguest::G(0x172fc8c)) = 0;
      uVar2 = 0x265b30;
LAB_00217338:
      RT_CALLRA(0x217338, guest::gs_tasks_spawn(param_1,uVar2,0));
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24)));
    }
    else {
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24)));
    }
    goto LAB_00217348;
  case 8:
  case 0xb:
    if ((*(uint32_t*)rtguest::G(0x1727b98)) == 0) {
      (*(uint32_t*)rtguest::G(0x172fca0)) = 0;
      (*(uint32_t*)rtguest::G(0x172fc9c)) = 0;
      uVar2 = 0x267248;
      goto LAB_00217338;
    }
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24)));
    goto LAB_00217348;
  case 9:
    if ((*(uint32_t*)rtguest::G(0x1727b98)) == 0) {
      (*(uint32_t*)rtguest::G(0x172fc98)) = 0;
      (*(uint32_t*)rtguest::G(0x172fc94)) = 0;
      uVar2 = 0x2665b8;
      goto LAB_00217338;
    }
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24)));
LAB_00217348:
    if (cVar1 != '\0') {
      (*(int32_t*)rtguest::G(0x1727b94)) = 2;
    }
    break;
  case 10:
  default:
    RT_CALLRA(0x217368, guest::UpdatePauseSubmenuTask(param_1,param_2,0x1727b90,0x172fc18));
  }
  (*(uint32_t*)rtguest::G(0x1727b98)) = (*(uint32_t*)rtguest::G(0x1727b98)) + 1;
  if ((*(uint32_t*)rtguest::G(0x1727b9c)) == 0) {
    return 0;
  }
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x18)));
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = 0xffffffff;
  gs_tasks_kill(param_1);
  return 0;
}


RT_GUEST_FN(0x00217148, guest::gs_options_pause_task)

}  // namespace guest
