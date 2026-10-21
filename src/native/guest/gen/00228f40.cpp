// guest:0x00228f40 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t LoadOrCreateCardSlotTask(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
  }
  char cVar1;
  undefined1 uVar2;
  int iVar3;
  int iVar4;
  
  iVar4 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 8))) == 0) {
    RT_CALLRA(0x228f64, guest::gs_tasks_spawn(param_1,0x2282c0,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c)))));
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x24)));
  }
  else {
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x24)));
  }
  if (cVar1 == '\0') {
    return 0;
  }
  switch((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25)))) {
  case 0:
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4)));
    if (iVar3 == 0) {
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c))) != 0) {
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c))) + 4;
        break;
      }
      RT_CALLRA(0x228fd0, guest::gs_tasks_spawn(param_1,0x2282c0,1));
      uVar2 = 4;
    }
    else {
      if (iVar3 < 0) {
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c)));
      }
      else {
        if (iVar3 < 3) {
          RT_CALLRA(0x229004, guest::gs_tasks_spawn(param_1,0x228510,(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c))) * 0x101));
          uVar2 = 1;
          goto LAB_002291b0;
        }
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c)));
      }
      if (iVar3 != 0) break;
      RT_CALLRA(0x22903c, guest::gs_tasks_spawn(param_1,0x2282c0,1));
      uVar2 = 3;
    }
    goto LAB_002291b0;
  case 1:
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))) == 0) {
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c))) == 0) {
        RT_CALLRA(0x2290c4, guest::gs_tasks_spawn(param_1,0x2282c0,1));
        uVar2 = 4;
        goto LAB_002291b0;
      }
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c))) + 4;
    }
    else {
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))) == 1) {
        iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x18)));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x24))) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))) = 1;
        gs_tasks_kill(param_1);
        return 0;
      }
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c))) + 8;
    }
    break;
  case 2:
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))) = 0;
    gs_tasks_kill(param_1);
    return 0;
  case 3:
    if (((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))) < 3) && (-1 < (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))))) {
      iVar3 = 0;
    }
    else {
      iVar3 = 6;
    }
    break;
  case 4:
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4)));
    if (iVar3 == 0) {
      iVar3 = 7;
    }
    else {
      if ((-1 < iVar3) && (iVar3 < 3)) {
        RT_CALLRA(0x229154, guest::gs_tasks_spawn(param_1,0x228510,0x101));
        uVar2 = 5;
        goto LAB_002291b0;
      }
LAB_0022919c:
      iVar3 = 4;
    }
    break;
  case 5:
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))) != 0) goto LAB_0022919c;
    iVar3 = 7;
    break;
  default:
    goto switchD_00228f98_default;
  }
  RT_CALLRA(0x2291a4, guest::gs_tasks_spawn(param_1,0x227380,iVar3));
  uVar2 = 2;
LAB_002291b0:
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25))) = uVar2;
switchD_00228f98_default:
  return 0;
}


RT_GUEST_FN(0x00228f40, guest::LoadOrCreateCardSlotTask)

}  // namespace guest
