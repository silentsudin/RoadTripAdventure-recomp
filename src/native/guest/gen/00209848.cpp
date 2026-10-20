// guest:0x00209848 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RunMenuSelectionTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  char cVar1;
  byte bVar2;
  int iVar3;
  int iVar4;
  undefined8 uVar5;
  long lVar6;
  int iVar7;
  int iVar8;
  
  iVar7 = (int)param_1;
  iVar8 = (int)param_2;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8))) == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x50))) = 0x80ffffff;
    gifBeginSpriteBatch(0x177a9e0,0x20043dce69329d8e);
    gifSetSpriteCount(0x177aa40,1);
    uiSpriteWriteFull(0x177aa50,0,0,0,0,0x280,0x1c0);
    (*(uint32_t*)rtguest::G(0x177aa94)) = 0xffff;
    gifSetTexRegs(0x177aaa0,0x20047b4e253221f2,0);
    gifSetSpriteCount(0x177aac0,1);
    uiSpriteWrite(0x177aad0,0x10,0x18,0,0,0x1b8,0xdc);
    gifChainEndTag(0x177a9d0,0x14);
    uVar5 = uiMenuCreate(param_1);
    uiMenuAddItem(uVar5,(*(uint32_t*)rtguest::G((uint32_t)(0x29acf8u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 3) * 4))),0x10);
    uiMenuAddItem(uVar5,(*(uint32_t*)rtguest::G((uint32_t)(0x29acfcu + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 3) * 4))),0x10);
    uiMenuAddItem(uVar5,(*(uint32_t*)rtguest::G((uint32_t)(0x29ad00u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 3) * 4))),0x10);
    CenterRectangle(uVar5,8);
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)uVar5)) = 0x17c;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((uint32_t)uVar5 + 6))) = 2;
  }
  sendTexturePackets(0x177a9d0);
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8))) < 0x20) {
    uiFade((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8))) << 0x19);
  }
  else {
    uiFade(0x40000000);
  }
  uiDrawHintBar(1);
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28)));
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x24))) == '\0') goto LAB_00209b18;
  bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25)));
  if (bVar2 == 1) {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 4))) == 1) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25))) = 2;
      gs_tasks_spawn(param_1,0x228f40,1);
      iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8)));
      goto LAB_00209b1c;
    }
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25))) = 0;
LAB_00209af0:
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xe))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xe))) & 0x7f;
  }
  else {
    if (bVar2 < 2) {
      if (bVar2 != 0) {
        iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8)));
        goto LAB_00209b1c;
      }
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 4))) == 0) {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25))) = 1;
        gs_tasks_spawn(param_1,0x228f40,0);
        iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8)));
        goto LAB_00209b1c;
      }
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25))) = 0;
      goto LAB_00209af0;
    }
    if (bVar2 == 2) {
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 4))) != 1) {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25))) = 0;
        goto LAB_00209af0;
      }
      PlayUiSound(0x303);
      gs_tasks_spawn(param_1,0x22a050,0);
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25))) = 3;
    }
    else {
      if (bVar2 != 3) {
        iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8)));
        goto LAB_00209b1c;
      }
      iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))) = 3;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))) = 0x400000;
      gs_tasks_kill(param_1);
    }
  }
LAB_00209b18:
  iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8)));
LAB_00209b1c:
  if (iVar4 != 0) {
    lVar6 = uiMenuRun(iVar3,param_2);
    if (lVar6 == 1) {
      PlayUiSound(0x1a);
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xd)));
      if (cVar1 == '\x01') {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 10))) = 3;
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xe))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xe))) | 0x80;
        gs_tasks_spawn(param_1,0x227380,2);
        return 0;
      }
      if (cVar1 < '\x02') {
        if (cVar1 == '\0') {
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 10))) = 2;
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))) = 0x400000;
          InitProgressRamDefaults();
          InitPlayerSlotStrings();
          iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x18)));
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x24))) = 1;
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 4))) = 3;
          gs_tasks_kill(param_1);
          return 0;
        }
      }
      else if (cVar1 == '\x02') {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 10))) = 6;
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xe))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xe))) | 0x80;
        gs_tasks_spawn(param_1,0x227380,2);
        return 0;
      }
    }
    else if (lVar6 == -1) {
      PlayUiSound(0x1b);
      iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 4))) = 0xffffffff;
      gs_tasks_kill(param_1);
      return 0;
    }
  }
  return 0;
}


RT_GUEST_FN(0x00209848, guest::RunMenuSelectionTask)

}  // namespace guest
