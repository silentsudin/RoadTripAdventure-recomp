// guest:0x0026b848 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t debugMenuTask(uint32_t param_1, uint32_t param_2, ...) {
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
  
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xf))) != '\0') {
    return 0;
  }
  iVar5 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x14))) = 81000;
    RT_CALLRA(0x26b8a0, guest::gs_tasks_spawn(param_1,0x20dc50,0x000a0000 + (*(uint8_t*)rtguest::G((uint32_t)0x0177f778 + 2))));
    RT_CALLRA(0x26b8b4, guest::gs_tasks_spawn(param_1,0x20dc50,0x300001));
    RT_CALLRA(0x26b8bc, guest::InitProgressRamDefaults());
    RT_CALLRA(0x26b8c4, guest::InitControlsBindings());
    (*(uint32_t*)rtguest::G(0x177ea58)) = 0x0177f798;
    (*(uint32_t*)rtguest::G(0x177ea54)) = 0x177f77c;
    (*(uint32_t*)rtguest::G(0x177ea50)) = 0x177f760;
    RT_CALLRA(0x26b8f4, guest::gs_tasks_spawn(param_1,0x20dc50,0x70003));
    uVar3 = RT_CALLRA(0x26b8fc, guest::uiMenuCreate(param_1));
    uiMenuAddItem(uVar3,0x3355f8,0x10);
    uiMenuAddItem(uVar3,0x32f920,0x10);
    uiMenuAddItem(uVar3,0x32f930,0x10);
    uiMenuAddItem(uVar3,0x32f940,0x10);
    uiMenuAddItem(uVar3,0x32f950,0x10);
    uiMenuAddItem(uVar3,0x32f960,0x10);
    uiMenuAddItem(uVar3,0x32f970,0x10);
    uiMenuAddItem(uVar3,0x32f980,0x10);
    uiMenuAddItem(uVar3,0x32f990,0x10);
    uiMenuAddItem(uVar3,0x32f9a0,0x10);
    uiMenuAddItem(uVar3,0x335600,0x10);
    uiMenuAddItem(uVar3,0x32f9b0,0x10);
    return 0;
  }
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24))) == '\0') {
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25)));
  }
  else {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25))) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10))) = 0;
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25)));
  }
  if (cVar1 != '\0') {
    return 0;
  }
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28)));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x50))) = 0x8033cc66;
  lVar4 = (int32_t)RT_CALLRA(0x26ba3c, guest::uiMenuRun(iVar2,param_2));
  if (lVar4 != 1) {
    return 0;
  }
  RT_CALLRA(0x26ba54, guest::PlayUiSound(0x1a));
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25))) = 1;
  switch((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xd)))) {
  case 0:
    uVar3 = 0x26c460;
    break;
  case 1:
    uVar3 = 0x26b1b8;
    break;
  case 2:
    uVar3 = 0x26b1b8;
    goto LAB_0026bbbc;
  case 3:
    uVar3 = 0x26b470;
    goto LAB_0026bbbc;
  case 4:
    uVar3 = 0x26b5c8;
    break;
  case 5:
    RT_CALLRA(0x26baec, guest::gs_tasks_spawn(param_1,0x20dc50,0x40005));
    RT_CALLRA(0x26bb00, guest::gs_tasks_spawn(param_1,0x20dc50,0x40017));
    uVar3 = 0x265b30;
    break;
  case 6:
    RT_CALLRA(0x26bb2c, guest::gs_tasks_spawn(param_1,0x20dc50,0x40005));
    RT_CALLRA(0x26bb40, guest::gs_tasks_spawn(param_1,0x20dc50,0x40018));
    uVar3 = 0x2665b8;
    break;
  case 7:
    RT_CALLRA(0x26bb68, guest::gs_tasks_spawn(param_1,0x20dc50,0x40005));
    uVar3 = 0x267248;
    break;
  case 8:
    uVar3 = 0x25bcf0;
    break;
  case 9:
    uVar3 = 0x26ccd0;
    break;
  case 10:
    uVar3 = 0x26c460;
LAB_0026bbbc:
    RT_CALLRA(0x26bb9c, guest::gs_tasks_spawn(param_1,uVar3,1));
    return 0;
  case 0xb:
    iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) = 0xffffffff;
    RT_CALLRA(0x26bbe4, guest::gs_tasks_kill(param_1));
  default:
    goto switchD_0026ba80_default;
  }
  RT_CALLRA(0x26bbbc, guest::gs_tasks_spawn(param_1,uVar3,0));
switchD_0026ba80_default:
  return 0;
}


RT_GUEST_FN(0x0026b848, guest::debugMenuTask)

}  // namespace guest
