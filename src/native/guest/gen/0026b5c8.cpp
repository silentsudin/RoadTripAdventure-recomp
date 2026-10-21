// guest:0x0026b5c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t actionStageSelectGridMenu(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x28, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x30, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x38) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x3c) = 0x00000000u;
    rtguest::setS32(2, (uint32_t)param_1);  // walk: = param_1
  }
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
  RT_CALLRA(0x26b628, guest::uiDrawPanelCells(0,0,0xf,0xb,0));
  RT_CALLRA(0x26b640, guest::uiDrawPanelCells(0xf,0,0x1e,0xb,0));
  bVar2 = RT_CALLRA(0x26b65c, guest::NavigateGridSelection(param_2,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x27))),2,10,0,1));
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x27))) = bVar2;
  if (bVar2 < 10) {
    RT_CALLRA(0x26b68c, guest::uiDrawBarWithArrow(0x1e,(uint32_t)bVar2 * 0x18 + 0x22,0x118));
  }
  else {
    RT_CALLRA(0x26b6b8, guest::uiDrawBarWithArrow(0x14a,(uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x27))) * 0x18 + -0xce,0x118));
  }
  iVar3 = 0;
  iVar5 = 0;
  do {
    iVar4 = iVar5 + 1;
    RT_CALLRA(0x26b6f4, guest::uiDrawTextCell(2,iVar4,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x2dccd8u + iVar3))),0));
    RT_CALLRA(0x26b720, guest::uiDrawTextCell(4,iVar4,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x2be398 + (uint32_t)((uint32_t)(*(uint8_t*)rtguest::G((uint32_t)0x2be018 + (uint32_t)(iVar5 * 0x10) * 1))) * 4)),0));
    iVar3 = iVar4 * 4;
    iVar5 = iVar4;
  } while (iVar4 < 10);
  iVar3 = 0;
  iVar5 = 0;
  do {
    iVar4 = iVar5 + 1;
    RT_CALLRA(0x26b76c, guest::uiDrawTextCell(0x11,iVar4,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x2dcd00u + iVar3))),0));
    RT_CALLRA(0x26b798, guest::uiDrawTextCell(0x13,iVar4,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x2be398 + (uint32_t)((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x2be0b8 + (uint32_t)(iVar5 * 0x10) * 4))) * 4)),0));
    iVar3 = iVar4 * 4;
    iVar5 = iVar4;
  } while (iVar4 < 10);
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) != 0) {
    uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x30)));
    if ((uVar1 & 0x40) == 0) {
      if ((uVar1 & 0x10) != 0) {
        RT_CALLRA(0x26b7c8, guest::PlayUiSound(0x1b));
        iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18)));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) = 1;
        RT_CALLRA(0x26b818, guest::gs_tasks_kill(param_1));
      }
    }
    else {
      RT_CALLRA(0x26b800, guest::PlayUiSound(0x1a));
      RT_CALLRA(0x26b7e0, guest::gs_tasks_spawn(param_1,0x20f950,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x27))) + 0x23));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) = 1;
    }
  }
  return 0;
}


RT_GUEST_FN(0x0026b5c8, guest::actionStageSelectGridMenu)

}  // namespace guest
