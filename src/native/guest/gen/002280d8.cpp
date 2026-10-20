// guest:0x002280d8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SaveSlotConfirmDialog(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
  }
  char cVar1;
  uint uVar2;
  byte bVar3;
  undefined8 uVar4;
  long lVar5;
uint32_t pcVar6;
  int iVar7;
  int iVar8;
  
  iVar8 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 8))) != 0) {
    RT_CALLRA(0x228240, guest::DrawSaveSlotEntry(0x177f760 + ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c))) & 0xf) * 0x689,4,0));
    iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
    lVar5 = RT_CALLRA(0x228250, guest::uiMenuRun(iVar7,param_2));
    if (lVar5 == 0) {
      return 0;
    }
    if (lVar5 == -1) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0xd))) = 1;
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0xd)));
    }
    else {
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0xd)));
    }
    iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x24))) = 1;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 4))) = (int)cVar1;
    gs_tasks_kill(param_1);
    return 0;
  }
  uVar4 = RT_CALLRA(0x228118, guest::uiMenuCreate(param_1) /* $a0 = entry $a0 at the jal (MIPS: no $4 write/jal before 0x228118); pseudocode dropped the live arg */);
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c))) & 0x100) == 0) {
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c))) & 0x200) == 0) {
      RT_CALLRA(0x228154, guest::uiMenuAddItem(uVar4,0x2eb4f8,0));
      goto LAB_00228190;
    }
    RT_CALLRA(0x228168, guest::uiMenuAddItem(uVar4,0x2eb4d8,0));
    pcVar6 = 0x2eb4f8u;
  }
  else {
    pcVar6 = 0x2eb4c0u;
  }
  RT_CALLRA(0x228184, guest::uiMenuAddItem(uVar4,pcVar6,0));
LAB_00228190:
  RT_CALLRA(0x228198, guest::uiMenuAddItem(uVar4,0x3335e8,0x10));
  RT_CALLRA(0x2281ac, guest::uiMenuAddItem(uVar4,0x3335e0,0x10));
  uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c)));
  iVar7 = (int)uVar4;
  if ((uVar2 & 0x200) != 0) {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0xd))) = 1;
    uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c)));
  }
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 2))) = 0x78;
  if ((uVar2 & 0x100) == 0) {
    bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c))) ^ 1;
  }
  else {
    bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c)));
  }
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0xc))) = bVar3 & 1;
  iVar8 = ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c))) & 0xf) * 0x3448;
  if ('\x03' < (char)(*(int8_t*)rtguest::G((uint32_t)0x177fdb1 + (uint32_t)(iVar8) * 1))) {
    (*(int8_t*)rtguest::G((uint32_t)0x177fdb1 + (uint32_t)(iVar8) * 1)) = 3;
  }
  return 0;
}


RT_GUEST_FN(0x002280d8, guest::SaveSlotConfirmDialog)

}  // namespace guest
