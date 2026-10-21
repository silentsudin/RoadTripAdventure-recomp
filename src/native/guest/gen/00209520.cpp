// guest:0x00209520 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// Pseudocode carries the GIF register payloads as undefined8; guest_decls truncates them.

uint32_t titleNewGameContinue(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x28) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x2c) = 0x00000000u;
  }
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
    RT_CALLRA(0x209580, guest::gifBeginSpriteBatch(0x177a9e0,0x20043dce69329d8eULL));
    RT_CALLRA(0x20958c, guest::gifSetSpriteCount(0x177aa40,1));
    RT_CALLRA(0x2095ac, guest::uiSpriteWriteFull(0x177aa50,0,0,0,0,0x280,0x1c0));
    (*(uint32_t*)rtguest::G(0x177aa94)) = 0xffff;
    RT_CALLRA(0x2095d8, guest::gifSetTexRegs(0x177aaa0,0x20047b4e253221f2ULL,0));
    RT_CALLRA(0x2095e4, guest::gifSetSpriteCount(0x177aac0,1));
    RT_CALLRA(0x209604, guest::uiSpriteWrite(0x177aad0,0x10,0x18,0,0,0x1b8,0xdc));
    RT_CALLRA(0x209610, guest::gifChainEndTag(0x177a9d0,0x14));
    uVar3 = RT_CALLRA(0x209618, guest::uiMenuCreate(param_1));
    RT_CALLRA(0x209638, guest::uiMenuAddItem(uVar3,(*(uint32_t*)rtguest::G((uint32_t)(0x29ace0u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 2) * 4))),0x10));
    RT_CALLRA(0x209654, guest::uiMenuAddItem(uVar3,(*(uint32_t*)rtguest::G((uint32_t)(0x29ace4u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 2) * 4))),0x10));
    RT_CALLRA(0x209660, guest::CenterRectangle(uVar3,8));
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)uVar3)) = 400;
  }
  RT_CALLRA(0x209670, guest::sendTexturePackets(0x177a9d0));
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) < 0x20) {
    RT_CALLRA(0x209688, guest::uiFade((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) << 0x19));
  }
  else {
    RT_CALLRA(0x209698, guest::uiFade(0x40000000));
  }
  RT_CALLRA(0x2096a0, guest::uiDrawHintBar(1));
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
    lVar4 = (int32_t)RT_CALLRA(0x20971c, guest::uiMenuRun(iVar2,param_2));
    if (lVar4 == 1) {
      RT_CALLRA(0x209730, guest::PlayUiSound(0x1a));
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xd)));
      if (cVar1 == '\x01') {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xe))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xe))) | 0x80;
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
      RT_CALLRA(0x2097f0, guest::PlayUiSound(0x1b));
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
