// guest:0x0026b5c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t actionStageSelectGridMenu(uint32_t param_1, uint32_t param_2, ...) {
  ushort uVar1;
  byte bVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  iVar6 = (int)param_1;
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) != '\0') {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) == '\0') {
      return 0;
    }
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) = 0;
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x50))) = 0x8033cc66;
  uiDrawPanelCells(0,0,0xf,0xb,0);
  uiDrawPanelCells(0xf,0,0x1e,0xb,0);
  bVar2 = NavigateGridSelection(param_2,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x27))),2,10,0,1);
  (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x27))) = bVar2;
  if (bVar2 < 10) {
    uiDrawBarWithArrow(0x1e,(uint32_t)bVar2 * 0x18 + 0x22,0x118);
  }
  else {
    uiDrawBarWithArrow(0x14a,(uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x27))) * 0x18 + -0xce,0x118);
  }
  iVar3 = 0;
  iVar5 = 0;
  do {
    iVar4 = iVar5 + 1;
    uiDrawTextCell(2,iVar4,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x2dccd8u + iVar3))),0);
    uiDrawTextCell(4,iVar4,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x2be398 + (uint32_t)((uint32_t)(*(uint8_t*)rtguest::G((uint32_t)0x2be018 + (uint32_t)(iVar5 * 0x10) * 1))) * 4)),0);
    iVar3 = iVar4 * 4;
    iVar5 = iVar4;
  } while (iVar4 < 10);
  iVar3 = 0;
  iVar5 = 0;
  do {
    iVar4 = iVar5 + 1;
    uiDrawTextCell(0x11,iVar4,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x2dcd00u + iVar3))),0);
    uiDrawTextCell(0x13,iVar4,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x2be398 + (uint32_t)((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x2be0b8 + (uint32_t)(iVar5 * 0x10) * 4))) * 4)),0);
    iVar3 = iVar4 * 4;
    iVar5 = iVar4;
  } while (iVar4 < 10);
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) != 0) {
    uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x30)));
    if ((uVar1 & 0x40) == 0) {
      if ((uVar1 & 0x10) != 0) {
        PlayUiSound(0x1b);
        iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18)));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) = 1;
        gs_tasks_kill(param_1);
      }
    }
    else {
      PlayUiSound(0x1a);
      gs_tasks_spawn(param_1,0x20f950,(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x27))) + 0x23);
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) = 1;
    }
  }
  return 0;
}


RT_GUEST_FN(0x0026b5c8, guest::actionStageSelectGridMenu)

}  // namespace guest
