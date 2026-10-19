// guest:0x00209520 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t titleNewGameContinue(uint32_t param_1, uint32_t param_2, ...) {
  char cVar1;
  int iVar2;
  undefined8 uVar3;
  long lVar4;
  int iVar5;
  int iVar6;
  
  iVar5 = (int)param_1;
  iVar6 = (int)param_2;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x50))) = 0x80ffffff;
    gifBeginSpriteBatch(0x177a9e0,0x20043dce69329d8e);
    gifSetSpriteCount(0x177aa40,1);
    uiSpriteWriteFull(0x177aa50,0,0,0,0,0x280,0x1c0);
    (*(uint32_t*)rtguest::G(0x177aa94)) = 0xffff;
    gifSetTexRegs(0x177aaa0,0x20047b4e253221f2,0);
    gifSetSpriteCount(0x177aac0,1);
    uiSpriteWrite(0x177aad0,0x10,0x18,0,0,0x1b8,0xdc);
    gifChainEndTag(0x177a9d0,0x14);
    uVar3 = uiMenuCreate(param_1);
    uiMenuAddItem(uVar3,(*(uint32_t*)rtguest::G((uint32_t)(0x29ace0u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 2) * 4))),0x10);
    uiMenuAddItem(uVar3,(*(uint32_t*)rtguest::G((uint32_t)(0x29ace4u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 2) * 4))),0x10);
    CenterRectangle(uVar3,8);
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)uVar3)) = 400;
  }
  sendTexturePackets(0x177a9d0);
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) < 0x20) {
    uiFade((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) << 0x19);
  }
  else {
    uiFade(0x40000000);
  }
  uiDrawHintBar(1);
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24))) != '\0') {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x18)));
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) == 1) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = 1;
    }
    else {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = 0xffffffff;
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) = 0x400000;
    gs_tasks_kill(param_1);
    return 0;
  }
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) != 0) {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28)));
    lVar4 = uiMenuRun(iVar2,param_2);
    if (lVar4 == 1) {
      PlayUiSound(0x1a);
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xd)));
      if (cVar1 == '\x01') {
        (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xe))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xe))) | 0x80;
        gs_tasks_spawn(param_1,0x228d10,0);
        return 0;
      }
      if (cVar1 < '\x02') {
        if (cVar1 == '\0') goto LAB_00209768;
      }
      else if (cVar1 == '\x02') {
LAB_00209768:
        iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x18)));
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) = 0x400000;
        cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xd)));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24))) = 1;
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) = (int)cVar1;
        gs_tasks_kill(param_1);
        return 0;
      }
    }
    else if (lVar4 == -1) {
      PlayUiSound(0x1b);
      iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) = 0xffffffff;
      gs_tasks_kill(param_1);
      return 0;
    }
  }
  return 0;
}


RT_GUEST_FN(0x00209520, guest::titleNewGameContinue)

}  // namespace guest
