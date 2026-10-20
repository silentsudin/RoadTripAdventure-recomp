// guest:0x00229538 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ShowSaveCreateSpinner(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
  int iVar1;
  int iVar2;
  undefined8 uVar3;
  long lVar4;
  undefined8 uVar5;
  int iVar6;
  
  iVar6 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) != 0) {
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)));
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) != '\0') {
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) == 1) {
        iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18)));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24))) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = 0;
        RT_CALLRA(0x229694, guest::gs_tasks_kill());
      }
      else {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0xe))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0xe))) & 0x7f;
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) = 0;
      }
    }
    lVar4 = RT_CALLRA(0x2296b8, guest::uiMenuRun(iVar1,param_2));
    if (lVar4 == 1) {
      RT_CALLRA(0x2296d0, guest::PlayUiSound(0x1a));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) = 1;
      RT_CALLRA(0x2296e8, guest::gs_tasks_spawn(param_1,0x2291c8,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0xd)))));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0xe))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0xe))) | 0x80;
      return 0;
    }
    if (lVar4 == -1) {
      RT_CALLRA(0x229708, guest::PlayUiSound(0x1b));
      iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) = 0;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0xe))) = 2;
      return 0;
    }
    if (lVar4 != -2) {
      return 0;
    }
    RT_CALLRA(0x229730, guest::gs_tasks_kill(param_1));
    return 0;
  }
  uVar3 = RT_CALLRA(0x229560, guest::uiMenuCreate(param_1) /* $a0 = entry $a0 at the jal (MIPS: no $4 write/jal before 0x229560); pseudocode dropped the live arg */);
  if ((*(uint32_t*)rtguest::G(0x333284)) == 1) {
    RT_CALLRA(0x2295b8, guest::uiMenuAddItem(uVar3,0x333648,0x10));
    uVar5 = 0x333650;
  }
  else if ((*(uint32_t*)rtguest::G(0x333284)) < 2) {
    if ((*(uint32_t*)rtguest::G(0x333284)) != 0) goto LAB_00229648;
    RT_CALLRA(0x2295dc, guest::uiMenuAddItem(uVar3,0x333638,0x10));
    uVar5 = 0x333640;
  }
  else {
    if ((*(uint32_t*)rtguest::G(0x333284)) != 2) {
      if ((*(uint32_t*)rtguest::G(0x333284)) == 3) {
        RT_CALLRA(0x2295fc, guest::uiMenuAddItem(uVar3,0x2eb820,0x10));
        RT_CALLRA(0x229610, guest::uiMenuAddItem(uVar3,0x2eb830,0x10));
      }
      goto LAB_00229648;
    }
    RT_CALLRA(0x229628, guest::uiMenuAddItem(uVar3,0x2eb800,0x10));
    uVar5 = 0x2eb810;
  }
  RT_CALLRA(0x22963c, guest::uiMenuAddItem(uVar3,uVar5,0x10));
LAB_00229648:
  RT_CALLRA(0x229648, guest::CenterRectangle(uVar3,5));
  iVar6 = (int)uVar3;
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x5f))) = 2;
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xe))) = 1;
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xd))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x24)));
  return 0;
}


RT_GUEST_FN(0x00229538, guest::ShowSaveCreateSpinner)

}  // namespace guest
