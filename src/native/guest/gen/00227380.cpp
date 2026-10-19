// guest:0x00227380 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t MemcardChooserDialogHandler(uint32_t param_1, uint32_t param_2, ...) {
  char cVar1;
  undefined8 uVar2;
  long lVar3;
uint32_t pcVar4;
  undefined8 uVar5;
  undefined8 uVar6;
  int iVar7;
  int iVar8;
  
  iVar8 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 8))) != 0) goto LAB_00227728;
  uVar2 = uiMenuCreate();
  iVar7 = (int)uVar2;
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0xc))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c))) & 1;
  switch((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c)))) {
  case 0:
    uiMenuAddItem(uVar2,0x2eb030,0);
    uVar5 = 0x2eb058;
    break;
  case 1:
    uiMenuAddItem(uVar2,0x2eb030,0);
    uVar5 = 0x2eb088;
    break;
  case 2:
    uiMenuAddItem(uVar2,0x2eb0b8,0);
    uiMenuAddItem(uVar2,0x2eb0e8,0);
    uiMenuAddItem(uVar2,0x2eb118,0);
    uiMenuAddItem(uVar2,0x3335d8,0x10);
    uVar5 = 0x3335e0;
    uVar6 = 0x10;
    goto LAB_00227560;
  case 3:
  case 10:
  case 0xb:
    goto switchD_002273dc_caseD_3;
  case 4:
    uiMenuAddItem(uVar2,0x2eb148,0);
    uVar5 = 0x2eb178;
    break;
  case 5:
    uiMenuAddItem(uVar2,0x2eb148,0);
    uVar5 = 0x2eb1a8;
    break;
  case 6:
    uiMenuAddItem(uVar2,0x2eb1d8,0);
    uVar5 = 0x2eb210;
    break;
  case 7:
    uiMenuAddItem(uVar2,0x2eb148,0);
    uVar5 = 0x2eb230;
    break;
  case 8:
    uVar5 = 0x2eb268;
    break;
  case 9:
    uVar5 = 0x2eb298;
    break;
  case 0xc:
  case 0xd:
    uiMenuAddItem(uVar2,0x2eb030,0);
    uiMenuAddItem(uVar2,0x2eb2c8,0);
    pcVar4 = 0x2eb2d8u;
    goto LAB_002276c4;
  case 0xe:
  case 0xf:
    uVar5 = 0x2eb2f0;
    goto LAB_00227694;
  case 0x10:
  case 0x11:
    uiMenuAddItem(uVar2,0x2eb030,0);
    uVar5 = 0x2eb300;
    goto LAB_00227694;
  case 0x12:
  case 0x13:
    uVar5 = 0x2eb318;
    goto LAB_00227694;
  case 0x14:
  case 0x15:
    uVar5 = 0x2eb328;
    goto LAB_00227694;
  case 0x16:
  case 0x17:
    uiMenuAddItem(uVar2,0x2eb338,0);
    uiMenuAddItem(uVar2,0x2eb350,0);
    uiMenuAddItem(uVar2,0x3335e8,0x10);
    uiMenuAddItem(uVar2,0x3335e0,0x10);
    iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 8)));
    goto LAB_00227720;
  case 0x18:
  case 0x19:
    uiMenuAddItem(uVar2,0x2eb378,0);
    uVar5 = 0x2eb398;
    goto LAB_00227694;
  case 0x1a:
  case 0x1b:
    uVar5 = 0x2eb338;
LAB_00227694:
    uiMenuAddItem(uVar2,uVar5,0);
    iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 8)));
    goto LAB_00227720;
  case 0x1c:
  case 0x1d:
    uiMenuAddItem(uVar2,0x2eb318,0);
    pcVar4 = 0x2eb3c8u;
LAB_002276c4:
    uiMenuAddItem(uVar2,pcVar4,0);
    uiMenuAddItem(uVar2,0x3335e8,0x10);
    uiMenuAddItem(uVar2,0x3335e0,0x10);
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0xd))) = 1;
    goto switchD_002273dc_caseD_3;
  case 0x1e:
  case 0x1f:
    uiMenuAddItem(uVar2,0x3335f0,0);
    goto switchD_002273dc_caseD_3;
  default:
    iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 8)));
    goto LAB_00227720;
  }
  uVar6 = 0;
LAB_00227560:
  uiMenuAddItem(uVar2,uVar5,uVar6);
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0xc))) = 2;
switchD_002273dc_caseD_3:
  iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 8)));
LAB_00227720:
  if (iVar7 != 0) {
LAB_00227728:
    iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
    lVar3 = uiMenuRun(iVar7,param_2);
    if (lVar3 != 0) {
      PlayUiSound(0x1a);
      if (lVar3 == -1) {
        (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0xd))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0xb))) + -1;
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
  }
  return 0;
}


RT_GUEST_FN(0x00227380, guest::MemcardChooserDialogHandler)

}  // namespace guest
