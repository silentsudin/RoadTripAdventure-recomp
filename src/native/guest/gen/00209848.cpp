// guest:0x00209848 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RunMenuSelectionTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x28) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x2c) = 0x00000000u;
    rtguest::setS32(2, (uint32_t)param_1);  // walk: = param_1
    rtguest::gS[3] = 0x000000000177a9d0ull;  // walk const
  }
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
    RT_CALLRA(0x2098a8, guest::gifBeginSpriteBatch(0x177a9e0,0x20043dce69329d8e));
    RT_CALLRA(0x2098b4, guest::gifSetSpriteCount(0x177aa40,1));
    RT_CALLRA(0x2098d4, guest::uiSpriteWriteFull(0x177aa50,0,0,0,0,0x280,0x1c0));
    (*(uint32_t*)rtguest::G(0x177aa94)) = 0xffff;
    RT_CALLRA(0x209900, guest::gifSetTexRegs(0x177aaa0,0x20047b4e253221f2,0));
    RT_CALLRA(0x20990c, guest::gifSetSpriteCount(0x177aac0,1));
    RT_CALLRA(0x20992c, guest::uiSpriteWrite(0x177aad0,0x10,0x18,0,0,0x1b8,0xdc));
    RT_CALLRA(0x209938, guest::gifChainEndTag(0x177a9d0,0x14));
    uVar5 = RT_CALLRA(0x209940, guest::uiMenuCreate(param_1));
    RT_CALLRA(0x209968, guest::uiMenuAddItem(uVar5,(*(uint32_t*)rtguest::G((uint32_t)(0x29acf8u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 3) * 4))),0x10));
    RT_CALLRA(0x20998c, guest::uiMenuAddItem(uVar5,(*(uint32_t*)rtguest::G((uint32_t)(0x29acfcu + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 3) * 4))),0x10));
    RT_CALLRA(0x2099b0, guest::uiMenuAddItem(uVar5,(*(uint32_t*)rtguest::G((uint32_t)(0x29ad00u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 3) * 4))),0x10));
    RT_CALLRA(0x2099bc, guest::CenterRectangle(uVar5,8));
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)uVar5)) = 0x17c;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((uint32_t)uVar5 + 6))) = 2;
  }
  RT_CALLRA(0x2099d4, guest::sendTexturePackets(0x177a9d0));
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8))) < 0x20) {
    RT_CALLRA(0x2099ec, guest::uiFade((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8))) << 0x19));
  }
  else {
    RT_CALLRA(0x209a00, guest::uiFade(0x40000000));
  }
  RT_CALLRA(0x209a08, guest::uiDrawHintBar(1));
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
      RT_CALLRA(0x209ac4, guest::PlayUiSound(0x303));
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
    lVar6 = (int32_t)RT_CALLRA(0x209b24, guest::uiMenuRun(iVar3,param_2));
    if (lVar6 == 1) {
      RT_CALLRA(0x209b3c, guest::PlayUiSound(0x1a));
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
          RT_CALLRA(0x209b84, guest::InitProgressRamDefaults());
          RT_CALLRA(0x209b8c, guest::InitPlayerSlotStrings());
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
      RT_CALLRA(0x209c58, guest::PlayUiSound(0x1b));
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
