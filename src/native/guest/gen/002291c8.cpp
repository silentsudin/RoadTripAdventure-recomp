// guest:0x002291c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CreateOrOverwriteSlotTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
  byte bVar1;
  undefined4 uVar2;
uint32_t pcVar3;
  int iVar4;
  int iVar5;
  
  iVar5 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) == 0) {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26))) = (char)((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c))) >> 8);
    RT_CALLRA(0x2291fc, guest::gs_tasks_spawn(param_1,0x2282c0,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c))) & 0xff));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25))) = 1;
  }
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24))) == '\0') {
    return 0;
  }
  switch((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25)))) {
  case 1:
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4)));
    if (iVar4 == 1) {
LAB_0022927c:
      RT_CALLRA(0x22928c, guest::gs_tasks_spawn(param_1,0x228510,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c))) ^ 0x100));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25))) = 5;
      return 0;
    }
    if (iVar4 < 2) {
      bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26)));
      if (iVar4 == 0) {
        RT_CALLRA(0x2292ac, guest::gs_tasks_spawn(param_1,0x227380,bVar1 + 0xc));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25))) = 2;
        return 0;
      }
    }
    else {
      if (iVar4 == 2) {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x27))) = 1;
        goto LAB_0022927c;
      }
      bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26)));
    }
    iVar4 = bVar1 + 0x10;
    break;
  case 2:
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) == 0) {
      RT_CALLRA(0x2292f0, guest::gs_tasks_spawn(param_1,0x22a218,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c)))));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25))) = 4;
      return 0;
    }
    iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x18)));
    goto LAB_002294a0;
  case 4:
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) == 0) {
LAB_002294c4:
      uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c)));
      pcVar3 = 0x228708;
LAB_002294cc:
      RT_CALLRA(0x229380, guest::gs_tasks_spawn(param_1,pcVar3,uVar2));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25))) = 7;
      return 0;
    }
    iVar4 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26))) + 0xe;
    break;
  case 5:
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4)));
    if (iVar4 == 0) {
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x27))) == '\0') goto switchD_0022923c_caseD_9;
      iVar4 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26))) + 0x18;
    }
    else {
      if (iVar4 < 1) {
        bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26)));
        if (iVar4 == -3) {
          iVar4 = bVar1 + 0x1c;
          goto LAB_002294f4;
        }
      }
      else {
        if (iVar4 == 1) {
          if ((*(uint32_t*)rtguest::G(0x1782b98)) == (*(int64_t*)rtguest::G(0x1785fe0))) {
            RT_CALLRA(0x22939c, guest::gs_tasks_spawn(param_1,0x2280d8,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26))) ^ 1));
          }
          else {
            RT_CALLRA(0x229450, guest::gs_tasks_spawn(param_1,0x2280d8,(uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26))) ^ 1 | 0x200));
          }
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25))) = 6;
          return 0;
        }
        bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26)));
      }
      iVar4 = bVar1 + 0x12;
    }
    break;
  case 6:
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) != 0) {
      RT_CALLRA(0x22940c, guest::PlayUiSound(0x1b));
      iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x18)));
      goto LAB_002294a0;
    }
    uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c)));
    pcVar3 = 0x22a168;
    goto LAB_002294cc;
  case 7:
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) == 1) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x24))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c)));
      RT_CALLRA(0x22947c, guest::gs_tasks_spawn(param_1,0x227380,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26))) + 0x1e));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25))) = 10;
      return 0;
    }
    iVar4 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26))) + 0x14;
    break;
  case 8:
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) == 0) goto LAB_002294c4;
  case 3:
    iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x18)));
LAB_002294a0:
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) = 0;
    RT_CALLRA(0x2294ac, guest::gs_tasks_kill(param_1));
    return 0;
  case 9:
switchD_0022923c_caseD_9:
    iVar4 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26))) + 0x16;
LAB_002294f4:
    RT_CALLRA(0x2294cc, guest::gs_tasks_spawn(param_1,0x227380,iVar4));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25))) = 8;
    return 0;
  case 10:
    iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) = 1;
    RT_CALLRA(0x229518, guest::gs_tasks_kill(param_1));
    return 0;
  default:
    goto switchD_0022923c_default;
  }
  RT_CALLRA(0x2294f4, guest::gs_tasks_spawn(param_1,0x227380,iVar4));
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25))) = 3;
switchD_0022923c_default:
  return 0;
}


RT_GUEST_FN(0x002291c8, guest::CreateOrOverwriteSlotTask)

}  // namespace guest
