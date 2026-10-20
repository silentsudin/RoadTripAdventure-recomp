// guest:0x0022a050 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RunConfirmDialogTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  undefined8 uVar1;
  long lVar2;
  int iVar3;
  
  iVar3 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) == 0) {
    uVar1 = uiMenuCreate();
    uiMenuAddItem(uVar1,0x2eb518,0);
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar1 + 0xc))) = 2;
    if ('\x03' < (*(int8_t*)rtguest::G(0x17831f9))) {
      (*(int8_t*)rtguest::G(0x17831f9)) = '\x03';
    }
    if ('\x03' < (*(int8_t*)rtguest::G(0x1786641))) {
      (*(int8_t*)rtguest::G(0x1786641)) = '\x03';
    }
  }
  else {
    DrawSaveSlotEntry(0x177f760,2,1);
    DrawSaveSlotEntry(0x1782ba8,10,2);
    lVar2 = uiMenuRun((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x28))),param_2);
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
