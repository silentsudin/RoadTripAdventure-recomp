// guest:0x0022cd38 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RunCourseSelectHubTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(112);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x60) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x64) = 0x00000000u;
  }
  undefined1 uVar1;
  int iVar2;
  char cVar3;
  undefined8 uVar4;
  long lVar5;
  int iVar6;
  int iVar7;
  undefined4 uVar8;
uint32_t auStack_70 = rtguest::allocFrame(64);
  
  iVar6 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) == 0) {
    RT_CALLRA(0x22cd6c, guest::UpdateSceneLightsAndFog(1,0));
    RT_CALLRA(0x22cd78, guest::vu0UnitMatrix(auStack_70));
    RT_CALLRA(0x22cd88, guest::SetViewDistanceGlobals(0x45000000,0x45000000));
    RT_CALLRA(0x22cda0, guest::buildCamera(0x177e6d0,0x29f3f0,auStack_70,auStack_70));
    RT_CALLRA(0x22cda8, guest::updateLightDirection(0x177e850));
    RT_CALLRA(0x22cdb8, guest::vu0UnitMatrix(0x1752660));
    uVar8 = RT_CALLRA(0x22cdd0, guest::atan2f(0x3f800000,0x40800000));
    RT_CALLRA(0x22cde0, guest::vu0RotMatrixY(uVar8,0x1752660,0x1752660));
    (*(float*)rtguest::G(0x1752690)) = -1.0;
    (*(uint32_t*)rtguest::G(0x1752698)) = -6.0;
    RT_CALLRA(0x22ce00, guest::vu0UnitMatrix(0x17526a0));
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x177ea50)) + 8))) & 0x400) == 0) {
      (*(uint32_t*)rtguest::G(0x17526d4)) = 0;
    }
    else {
      (*(uint32_t*)rtguest::G(0x17526d4)) = (*(uint32_t*)rtguest::G(0x332e28));
    }
    RT_CALLRA(0x22ce2c, guest::PlayUiSound(0x32));
    RT_CALLRA(0x22ce38, guest::gs_music_play_bgm(0xa0b0102));
    RT_CALLRA(0x22ce40, guest::PlayBgmSlot1());
    (*(uint32_t*)rtguest::G(0x177ae8c)) = 0x40000;
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x177ea50)) + 8))) & 8) != 0) {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x177ea50)) + 8))) = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x177ea50)) + 8))) & 0xfff7 | 4;
    }
    uVar4 = RT_CALLRA(0x22ce70, guest::uiMenuCreate(param_1));
    RT_CALLRA(0x22ce88, guest::uiMenuAddItem(uVar4,0x2ebcb0,0x10));
    RT_CALLRA(0x22ce9c, guest::uiMenuAddItem(uVar4,0x2ebcc0,0x10));
    RT_CALLRA(0x22ceb0, guest::uiMenuAddItem(uVar4,0x3336f0,0x10));
    RT_CALLRA(0x22cec4, guest::uiMenuAddItem(uVar4,0x2ebcd0,0x10));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar4 + 0x5f))) = 2;
    RT_CALLRA(0x22ced8, guest::CenterRectangle(uVar4,5));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar4 + 0xe))) = 1;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26))) = (*(uint32_t*)rtguest::G(0x177fdb2));
    RT_CALLRA(0x22cef4, guest::ReleaseStampFlag(param_2));
  }
  cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25)));
  iVar7 = (int)param_2;
  if (cVar3 == '\x05') {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) | 0x400000;
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0xf))) == '\0') {
      cVar3 = '\x02';
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) = 2;
    }
    else {
      cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25)));
    }
  }
  if (cVar3 == '\0') {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) < 0x32) {
      (*(float*)rtguest::G(0x1752690)) = (*(float*)rtguest::G(0x1752690)) + (*(uint32_t*)rtguest::G(0x332e2c));
      (*(uint32_t*)rtguest::G(0x1752698)) = (*(uint32_t*)rtguest::G(0x1752698)) + (*(uint32_t*)rtguest::G(0x332e30));
    }
    else {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) = 1;
    }
    cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24)));
  }
  else {
    cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24)));
  }
  if (cVar3 != '\0') {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) == '\x02') {
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) != -1) {
        cVar3 = RT_CALLRA(0x22cfc0, guest::CallFindFlaggedSlot(0xf,0));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26))) = cVar3;
        if ((*(uint32_t*)rtguest::G(0x177fdb2)) != cVar3) {
          (*(uint32_t*)rtguest::G(0x177fdb2)) = cVar3;
          RT_CALLRA(0x22cffc, guest::gs_tasks_spawn(param_1,0x20dc50,(uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26))) | 0x30000));
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) = 5;
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) | 0x400000;
        }
      }
    }
    else {
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) != '\x03') {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) = 1;
        goto LAB_0022d0a8;
      }
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) != -1) {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x23))) = (*(uint8_t*)rtguest::G((uint32_t)0x29f440 + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) * 2) * 1));
        uVar1 = (*(uint32_t*)rtguest::G((uint32_t)0x29f441 + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) * 2) * 4));
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) | 0x400000;
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x21))) = uVar1;
        iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18)));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x24))) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 4))) = 0xb;
        RT_CALLRA(0x22d07c, guest::drawModelAt177F358((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x20)))));
        RT_CALLRA(0x22d084, guest::gs_tasks_kill(param_1));
        RT_CALLRA(0x22d08c, guest::PlayBgmSlot5());
        return 0;
      }
    }
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) = 1;
  }
LAB_0022d0a8:
  RT_CALLRA(0x22d0a8, guest::drawModelAt177F358((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x20)))));
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) == '\x01') {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)));
    lVar5 = RT_CALLRA(0x22d0c8, guest::uiMenuRun(iVar2,param_2));
    if (lVar5 != 0) {
      if (lVar5 == -2) {
        iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18)));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24))) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = 10;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xc))) = 0x22c418;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) | 0x400000;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) = 0;
        RT_CALLRA(0x22d1bc, guest::PlayBgmSlot5());
      }
      else if ((lVar5 == -1) ||
              (lVar5 = (long)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xd))), lVar5 == (int)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xb))) - 1))) {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xe))) = 2;
        RT_CALLRA(0x22d104, guest::PlayUiSound(0x1a));
      }
      else if (lVar5 == 0) {
        RT_CALLRA(0x22d128, guest::gs_tasks_spawn(param_1,0x264498,0));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) = 4;
      }
      else if (lVar5 == 1) {
        RT_CALLRA(0x22d154, guest::gs_tasks_spawn(param_1,0x22c8a0,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26)))));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) = 2;
      }
      else if (lVar5 == 2) {
        RT_CALLRA(0x22d17c, guest::gs_tasks_spawn(param_1,0x22ca28,0));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) = 3;
      }
    }
  }
  return 0;
}


RT_GUEST_FN(0x0022cd38, guest::RunCourseSelectHubTask)

}  // namespace guest
