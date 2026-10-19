// guest:0x00214ad0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdatePauseMenuTask(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  long lVar1;
uint32_t puVar2;
uint32_t puVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  uiDrawHintBar(2);
  iVar5 = (int)param_3;
  puVar2 = (uint32_t)param_4;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)puVar2)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(1) * sizeof(uint32_t))) = 0;
    InitMenuElementLayout(param_3,iVar5 + 0x820c,puVar2 + 7,(*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(0x17) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(2) * sizeof(uint32_t))));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81d8))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81cc))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81d4))) = 0xfffff0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81d0))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81c8))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81c8))) | 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x8208))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81e0))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81e4))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81e8))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81ec))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81f0))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81f8))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81fc))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x8200))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x8204))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81dc))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81dc))) | 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81f4))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81f4))) | 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81c0))) = 2;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81b8))) = 0x1b8;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81bc))) = 0xa0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81b4))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81b4))) | 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81a4))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81a4))) | 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81b0))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81a8))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81ac))) = 0;
  }
  lVar1 = UpdateVerticalMenuNavigation(param_1,param_2,param_3,param_4);
  if (lVar1 == 299) {
    switch((*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)((*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(2) * sizeof(uint32_t))) + 8) * sizeof(uint32_t)))) {
    case 1:
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) = 1;
      break;
    case 2:
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) = 2;
      break;
    case 3:
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) = 3;
      break;
    case 4:
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) = 4;
      break;
    case 5:
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) = 5;
      break;
    case 6:
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) = 6;
      break;
    default:
      goto switchD_00214d40_default;
    }
  }
  else {
    if (lVar1 < 300) {
      if (lVar1 == 200) {
        iVar4 = (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)((*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(2) * sizeof(uint32_t))) + 8) * sizeof(uint32_t)));
        if (iVar4 == 5) {
          gs_tasks_spawn(param_1,0x20dc50,0x3c0300);
          gs_tasks_spawn(param_1,0x20dc50,0x34000e);
        }
        else if (iVar4 < 6) {
          if (iVar4 == 2) {
            gs_tasks_spawn(param_1,0x20dc50,0x340005);
          }
        }
        else if (iVar4 == 6) {
          gs_tasks_spawn(param_1,0x20dc50,0x340016);
        }
      }
      goto LAB_00214db8;
    }
    if (lVar1 != 399) goto LAB_00214db8;
switchD_00214d40_default:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xc))) = 1;
  }
LAB_00214db8:
  iVar4 = iVar5 + 0x20;
  puVar3 = (uint32_t)(iVar5 + 0x81dc);
  uiArenaInit(iVar4,0x8000);
  uiArenaBeginBatch(iVar4,0,0);
  iVar6 = 1;
  DrawMenuGridBorder(param_3,iVar4,iVar5 + 0x81a4,1);
  DrawMenuListItems(param_3,iVar4,iVar5 + 0x81b4,1,param_2);
  DrawMenuGridBackground(param_3,iVar4,iVar5 + 0x81c8,1);
  do {
    if (((((*(uint32_t*)rtguest::G((uint32_t)puVar3)) & 1) != 0) && ((*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(4) * sizeof(uint32_t))) != 0)) && (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) & 0x10) != 0)) {
      uiArenaSetTexture(iVar4,0x29c030,0,0);
      uiArenaDataTag(iVar4,1);
      uiArenaSprites(iVar4,(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(5) * sizeof(uint32_t))) * 0x20 + 0x29c300,1,(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(3) * sizeof(uint32_t))));
    }
    iVar6 = iVar6 + -1;
    puVar3 = puVar3 + 6;
  } while (iVar6 != -1);
  DrawMenuCursorAndIcons(param_3,iVar4,iVar5 + 0x820c,0xb);
  uiArenaSubmit(iVar4);
  return 0;
}


RT_GUEST_FN(0x00214ad0, guest::UpdatePauseMenuTask)

}  // namespace guest
