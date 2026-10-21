// guest:0x0022a050 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RunConfirmDialogTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
  }
  undefined8 uVar1;
  long lVar2;
  int iVar3;
  
  iVar3 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) == 0) {
    uVar1 = RT_CALLRA(0x22a078, guest::uiMenuCreate(param_1) /* $a0 = entry $a0 at the jal (MIPS: no $4 write/jal before 0x22a078); pseudocode dropped the live arg */);
    RT_CALLRA(0x22a090, guest::uiMenuAddItem(uVar1,0x2eb518,0));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar1 + 0xc))) = 2;
    if ('\x03' < (*(int8_t*)rtguest::G(0x17831f9))) {
      (*(int8_t*)rtguest::G(0x17831f9)) = '\x03';
    }
    if ('\x03' < (*(int8_t*)rtguest::G(0x1786641))) {
      (*(int8_t*)rtguest::G(0x1786641)) = '\x03';
    }
  }
  else {
    RT_CALLRA(0x22a0f4, guest::DrawSaveSlotEntry(0x177f760,2,1));
    RT_CALLRA(0x22a104, guest::DrawSaveSlotEntry(0x1782ba8,10,2));
    lVar2 = (int32_t)RT_CALLRA(0x22a110, guest::uiMenuRun((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x28))),param_2));
    if (lVar2 == 1) {
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = 1;
      gs_tasks_kill(param_1);
      return 0;
    }
  }
  return 0;
}


RT_GUEST_FN(0x0022a050, guest::RunConfirmDialogTask)

}  // namespace guest
