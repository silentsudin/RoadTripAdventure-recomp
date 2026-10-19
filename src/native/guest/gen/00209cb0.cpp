// guest:0x00209cb0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RunMenuNavigationTask(uint32_t param_1, uint32_t param_2, ...) {
  ushort uVar1;
  int iVar2;
  undefined8 uVar3;
  long lVar4;
  byte bVar5;
  int iVar6;
  int iVar7;
  
  iVar7 = (int)param_2;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x50))) = 0x80000000;
  iVar6 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x50))) = 0x80ffffff;
    gifBeginSpriteBatch(0x177a9e0,0x20043dce69329d8e);
    gifSetSpriteCount(0x177aa40,1);
    uiSpriteWriteFull(0x177aa50,0,0,0,0,0x280,0x1c0);
    (*(uint32_t*)rtguest::G(0x177aa94)) = 0xffff;
    gifSetTexRegs(0x177aaa0,0x20047b4e253221f2,0);
    gifSetSpriteCount(0x177aac0,1);
    uiSpriteWrite(0x177aad0,0x10,0x18,0,0,0x1b8,0xdc);
    gifChainEndTag(0x177a9d0,0x14);
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x27))) = (uint32_t)(*(uint32_t*)rtguest::G(0x333284));
  }
  sendTexturePackets(0x177a9d0);
  bVar5 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0xc))) != '\0';
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0xd))) != '\0') {
    bVar5 = bVar5 | 2;
  }
  if (((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) == 0) ||
     (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26))) == '\0' && ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) != bVar5)))) {
    DestroyTaskRecord(param_1);
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) = bVar5;
    uVar3 = uiMenuCreate(param_1);
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0xc))) == '\0') {
      uiMenuAddItem(uVar3,0x2ea360,0);
    }
    else {
      uiMenuAddItem(uVar3,(*(uint32_t*)rtguest::G((uint32_t)(0x29ad20u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 5) * 4))),0x10);
      uiMenuAddItem(uVar3,(*(uint32_t*)rtguest::G((uint32_t)(0x29ad24u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 5) * 4))),0x10);
      if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0xd))) != '\0') && (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) & 1) != 0)) {
        uiMenuAddItem(uVar3,(*(uint32_t*)rtguest::G((uint32_t)(0x29ad28u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 5) * 4))),0x10);
      }
      uiMenuAddItem(uVar3,(*(uint32_t*)rtguest::G((uint32_t)(0x29ad2cu + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 5) * 4))),0x10);
      uiMenuAddItem(uVar3,(*(uint32_t*)rtguest::G((uint32_t)(0x29ad30u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 5) * 4))),0x10);
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar3 + 0xc))) = 2;
    }
    CenterRectangle(uVar3,10);
  }
  uiDrawHintBar(0);
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) == 0) {
    return 0;
  }
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)));
  if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) != '\0') && ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) == 1)) {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26))) = 0;
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xe))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xe))) & 0x7f;
  }
  lVar4 = uiMenuRun(iVar2,param_2);
  if (lVar4 != 1) {
    return 0;
  }
  switch((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xd)))) {
  case 0:
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x30))) & 0x40) == 0) {
      PlayUiSound(0x1c);
      return 0;
    }
    PlayUiSound(0x1a);
    gs_tasks_kill(param_1);
    iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 10))) = 0;
    goto LAB_0020a028;
  case 1:
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x30))) & 0x40) == 0) {
      PlayUiSound(0x1c);
      return 0;
    }
    PlayUiSound(0x1a);
    gs_tasks_kill(param_1);
    iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) = 1;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 10))) = 1;
LAB_0020a028:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) | 0x400000;
    return 0;
  case 2:
    if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) & 2) != 0) {
      PlayUiSound(0x1a);
      gs_tasks_kill(param_1);
      iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) = 2;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 10))) = 2;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) | 0x400000;
      return 0;
    }
    PlayUiSound(0x1a);
    uVar3 = 0x2092d8;
    break;
  case 3:
    if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) & 2) == 0) {
      uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x30)));
      goto LAB_0020a0ec;
    }
    PlayUiSound(0x1a);
    uVar3 = 0x2092d8;
    break;
  case 4:
    uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x30)));
LAB_0020a0ec:
    if ((uVar1 & 0x40) == 0) {
      PlayUiSound(0x1c);
      return 0;
    }
    PlayUiSound(0x1a);
    uVar3 = 0x20a580;
    break;
  default:
    goto switchD_00209f88_default;
  }
  gs_tasks_spawn(param_1,uVar3,0);
  (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xe))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xe))) | 0x80;
switchD_00209f88_default:
  return 0;
}


RT_GUEST_FN(0x00209cb0, guest::RunMenuNavigationTask)

}  // namespace guest
