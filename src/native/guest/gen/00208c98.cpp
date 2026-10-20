// guest:0x00208c98 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gs_options_sub_tasks_vibration(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
  char cVar1;
  byte bVar2;
  int iVar3;
  byte bVar4;
  undefined8 uVar5;
  long lVar6;
  byte bVar7;
  byte bVar8;
  int iVar9;
  int iVar10;
  
  iVar10 = (int)param_1;
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x25))) == '\0') {
    uVar5 = RT_CALLRA(0x208cc0, guest::uiMenuCreate(param_1) /* $a0 = entry $a0 at the jal (MIPS: no $4 write/jal before 0x208cc0); pseudocode dropped the live arg */);
    RT_CALLRA(0x208cec, guest::uiMenuAddItem(uVar5,(*(uint32_t*)rtguest::G((uint32_t)(0x29ac40u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 5) * 4))),0));
    RT_CALLRA(0x208d10, guest::uiMenuAddItem(uVar5,(*(uint32_t*)rtguest::G((uint32_t)(0x29ac44u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 5) * 4))),0x10));
    RT_CALLRA(0x208d34, guest::uiMenuAddItem(uVar5,(*(uint32_t*)rtguest::G((uint32_t)(0x29ac48u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 5) * 4))),0x10));
    RT_CALLRA(0x208d58, guest::uiMenuAddItem(uVar5,(*(uint32_t*)rtguest::G((uint32_t)(0x29ac4cu + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 5) * 4))),0x10));
    RT_CALLRA(0x208d7c, guest::uiMenuAddItem(uVar5,(*(uint32_t*)rtguest::G((uint32_t)(0x29ac50u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 5) * 4))),0x10));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar5 + 0xc))) = 2;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x25))) = 1;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar5 + 0xe))) = 1;
  }
  else {
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28)));
    bVar8 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x51))) & 0xfd;
    bVar7 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x52))) & 0xfd;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x51))) = bVar8;
    bVar4 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x53))) & 0xfd;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x52))) = bVar7;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x53))) = bVar4;
    iVar9 = (int)param_2;
    bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0xe)));
    if (bVar2 == 2) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x53))) = bVar4 | 2;
    }
    else if (bVar2 < 3) {
      if (bVar2 == 0) {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x52))) = bVar7 | 2;
      }
    }
    else if (bVar2 == 4) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x51))) = bVar8 | 2;
    }
    lVar6 = RT_CALLRA(0x208e28, guest::uiMenuRun(iVar3,param_2));
    if (lVar6 == 1) {
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xd)));
      if (cVar1 == '\x01') {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0xe))) = 0;
      }
      else if (cVar1 < '\x02') {
        if (cVar1 == '\0') {
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0xe))) = 4;
        }
      }
      else if (cVar1 == '\x02') {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0xe))) = 2;
      }
      else if (cVar1 == '\x03') {
        RT_CALLRA(0x208ea0, guest::gs_controls_rumble_start(param_2,0,7,1));
        RT_CALLRA(0x208eb4, guest::gs_controls_rumble_start(param_2,1,7,1));
      }
      PlayUiSound(0x1a);
      return 0;
    }
    if (lVar6 == -1) {
      PlayUiSound(0x1b);
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xe))) = 2;
      iVar10 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 4))) = 1;
    }
    else if (lVar6 == -2) {
      gs_tasks_kill(param_1);
      return 0;
    }
  }
  return 0;
}


RT_GUEST_FN(0x00208c98, guest::gs_options_sub_tasks_vibration)

}  // namespace guest
