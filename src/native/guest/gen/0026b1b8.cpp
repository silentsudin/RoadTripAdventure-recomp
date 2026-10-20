// guest:0x0026b1b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t raceCourseSelectScene(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x48) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4c) = 0x00000000u;
  }
  byte bVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  iVar4 = (int)param_1;
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25))) != '\0') {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x24))) == '\0') {
      return 0;
    }
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25))) = 0;
  }
  iVar5 = (int)param_2;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x50))) = 0x8033cc66;
  RT_CALLRA(0x26b220, guest::uiDrawPanelCells(0,0,0xf,0xf,0));
  RT_CALLRA(0x26b238, guest::uiDrawPanelCells(0xf,0,0x1e,0xf,0));
  bVar1 = RT_CALLRA(0x26b254, guest::NavigateGridSelection(param_2,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x27))),2,0xe,0,1));
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x27))) = bVar1;
  if (bVar1 < 0xe) {
    RT_CALLRA(0x26b284, guest::uiDrawBarWithArrow(0x32,(uint32_t)bVar1 * 0x18 + 0x22,0x104));
  }
  else {
    RT_CALLRA(0x26b2b0, guest::uiDrawBarWithArrow(0x15e,(uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x27))) * 0x18 + -0x12e,0x104));
  }
  iVar3 = 0;
  iVar2 = 0;
  while( true ) {
    iVar3 = iVar3 + 1;
    RT_CALLRA(0x26b308, guest::uiDrawTextCell(2,iVar3,(*(uint32_t*)rtguest::G((uint32_t)(0x2dcc78u + (uint32_t)((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x2bddd0u + (uint32_t)(iVar2) * 4))) * 4))),0));
    RT_CALLRA(0x26b328, guest::uiDrawTextCell(4,iVar3,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x2be398 + (uint32_t)((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x2bddd0 + (uint32_t)(iVar2) * 4))) * 4)),0))
    ;
    RT_CALLRA(0x26b348, guest::uiDrawTextCell(0xe,iVar3,(*(uint32_t*)rtguest::G((uint32_t)(0x2dccc0u + (uint32_t)((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x002bddd3u + (uint32_t)(iVar2) * 4))) * 4))),5));
    RT_CALLRA(0x26b368, guest::uiDrawTextCell(0x11,iVar3,(*(uint32_t*)rtguest::G((uint32_t)(0x2dcc78u + (uint32_t)((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x002bdeb0u + (uint32_t)(iVar2) * 4))) * 4))),0));
    RT_CALLRA(0x26b388, guest::uiDrawTextCell(0x13,iVar3,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x2be398 + (uint32_t)((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x2bdeb0 + (uint32_t)(iVar2) * 4))) * 4)),0));
    RT_CALLRA(0x26b3a8, guest::uiDrawTextCell(0x1d,iVar3,(*(uint32_t*)rtguest::G((uint32_t)(0x2dccc0u + (uint32_t)((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x002bdeb3u + (uint32_t)(iVar2) * 4))) * 4))),5));
    if (0xd < iVar3) break;
    iVar2 = iVar3 * 0x10;
  }
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 8))) != 0) {
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x30))) & 0x40) == 0) {
      if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x30))) & 0x10) != 0) {
        RT_CALLRA(0x26b3d8, guest::PlayUiSound(0x1b));
        iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x18)));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x24))) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))) = 1;
        RT_CALLRA(0x26b434, guest::gs_tasks_kill(param_1));
      }
    }
    else {
      RT_CALLRA(0x26b41c, guest::PlayUiSound(0x1a));
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c))) == 1) {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28))) = 0x800;
      }
      RT_CALLRA(0x26b3fc, guest::gs_tasks_spawn(param_1,0x20f950,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x27)))));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25))) = 1;
    }
  }
  return 0;
}


RT_GUEST_FN(0x0026b1b8, guest::raceCourseSelectScene)

}  // namespace guest
