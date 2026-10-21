// guest:0x00263e68 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t distanceGoalRaceTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x50) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x54) = 0x00000000u;
  }
uint32_t auStack_60 = rtguest::allocFrame(4);
    uint32_t fStack_5c = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
uint32_t auStack_50 = rtguest::allocFrame(4);
uint32_t auStack_4c = rtguest::allocFrame(12);
  int iVar1;
  undefined4 uVar2;
  bool bVar3;
uint32_t pvVar4;
  undefined8 uVar5;
  long lVar6;
  int iVar7;
  int iVar8;
uint32_t puVar9;
  int iVar10;
  float fVar11;
  
  iVar8 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 8))) == 0) {
    uVar5 = RT_CALLRA(0x263e98, guest::getExtraPacketArea(0x8cc0));
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))) = (int)uVar5;
    RT_CALLRA(0x263ea4, guest::InitObjectType23(uVar5));
    RT_CALLRA(0x263eb0, guest::gs_music_play_bgm(0xa100201));
    RT_CALLRA(0x263ec4, guest::gs_tasks_spawn(param_1,0x2357c0,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)))));
    puVar9 = (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))) + 0x8cb0);
    pvVar4 = (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(puVar9)), 0, 0x10));
    (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 0;
    return pvVar4;
  }
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
  puVar9 = (uint32_t)(iVar1 + 0x8cb0);
  iVar10 = (int)param_2;
  iVar7 = iVar1;
  switch((*(uint32_t*)rtguest::G((uint32_t)puVar9))) {
  case 0:
    RT_CALLRA(0x263f28, guest::SelectMeshIntersectionRoutine(1));
    RT_CALLRA(0x263f44, guest::StoreIndexedPosition((*(uint32_t*)rtguest::G(0x3331d0)),0x43b60000,(*(uint32_t*)rtguest::G(0x3331d4)),0,0xffffffffffff8000));
    (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb8))) = 0;
  case 1:
    fVar11 = (*(float*)rtguest::G(0x177ace8));
    iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb8)));
    if (iVar7 == 1) {
      RT_CALLRA(0x264038, guest::vu0CopyVector(auStack_60,0x177ace0));
      RT_CALLRA(0x00264054, rtCall((*(uint32_t*)rtguest::G(0x335710)), auStack_60, auStack_50, auStack_4c, 0, param_2));
      bVar3 = (*(float*)rtguest::G(0x177ace4)) <= (*(float*)rtguest::G((uint32_t)fStack_5c));
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cbc))) = (int)((*(float*)rtguest::G(0x3331e4)) - (*(float*)rtguest::G(0x177ace8)));
      if (bVar3) {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb8))) = 2;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) & 0xfffffeff;
        uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
      }
      else {
        uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
      }
      RT_CALLRA(0x264018, guest::SetObjectFields847C8470(uVar2,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cbc))),1));
      iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
    }
    else if (iVar7 < 2) {
      if (iVar7 == 0) {
        if ((*(float*)rtguest::G(0x177ace8)) <= (*(float*)rtguest::G(0x3331d8))) {
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb8))) = 1;
        }
        if ((1280.0 <= fVar11) && ((*(float*)rtguest::G(0x177ace0)) <= (*(float*)rtguest::G(0x3331dc)))) {
          RT_CALLRA(0x263fe0, guest::GetOrSetPresidentRaceResult(0));
          (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 2;
          (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) | 0x800;
        }
        RT_CALLRA(0x2640a4, guest::SetObjectFields847C8470((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))),(int)((*(float*)rtguest::G(0x3331e0)) - (*(float*)rtguest::G(0x177ace8))),1));
        iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
      }
      else {
        iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
      }
    }
    else if (iVar7 == 2) {
      fVar11 = rtguest::fbits(RT_CALLRA(0x2640bc, guest::GetSpeedFromF15Vector(0x177ad40)));
      if ((int)fVar11 < 2) {
        (*(uint32_t*)rtguest::G(0x177ad40)) = 0;
        (*(uint32_t*)rtguest::G(0x177ad48)) = 0;
      }
      if ((*(uint32_t*)rtguest::G(0x177ad40)) < 0) {
        (*(uint32_t*)rtguest::G(0x177ad40)) = (*(uint32_t*)rtguest::G(0x177ad40)) + 0x98;
      }
      if (0 < (*(uint32_t*)rtguest::G(0x177ad40))) {
        (*(uint32_t*)rtguest::G(0x177ad40)) = (*(uint32_t*)rtguest::G(0x177ad40)) + -0x98;
      }
      if ((*(uint32_t*)rtguest::G(0x177ad48)) < 0) {
        (*(uint32_t*)rtguest::G(0x177ad48)) = (*(uint32_t*)rtguest::G(0x177ad48)) + 0x98;
      }
      if (0 < (*(uint32_t*)rtguest::G(0x177ad48))) {
        (*(uint32_t*)rtguest::G(0x177ad48)) = (*(uint32_t*)rtguest::G(0x177ad48)) + -0x98;
      }
      (*(uint32_t*)rtguest::G(0x177ae4e)) = 1;
      (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) | 0x800;
      if ((*(uint32_t*)rtguest::G(0x177ad40)) == 0) {
        if ((*(uint32_t*)rtguest::G(0x177ad48)) != 0) {
          iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
          break;
        }
        lVar6 = RT_CALLRA(0x264150, guest::UpdateMaxCounter847C((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)))));
        if (((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cbc))) < 0x8c) ||
           ((lVar6 == 0 && (lVar6 = TestStateBitFc78(0x42), lVar6 != 0)))) {
          RT_CALLRA(0x264184, guest::GetOrSetPresidentRaceResult(1));
          RT_CALLRA(0x2641b8, guest::SetBumperConfig((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))),0,1));
        }
        else {
          RT_CALLRA(0x2641a8, guest::GetOrSetPresidentRaceResult(2));
          RT_CALLRA(0x264194, guest::SetRoadData2((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))),0,1));
        }
        (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 2;
        (*(uint32_t*)rtguest::G(0x177ae4e)) = 0;
      }
      iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
    }
    else {
      iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
    }
    break;
  case 2:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) | 8;
    (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 3;
    break;
  case 3:
    RT_CALLRA(0x2641f4, guest::uiFade((int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))) << 0x18));
    if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))) < 0x80) {
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))) + 1;
      iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
    }
    else {
      iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
      (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 0xffffffff;
    }
    break;
  case 0xffffffff:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) = 1;
  }
  RT_CALLRA(0x264244, guest::raceHudDraw(iVar7));
  pvVar4 = (uint32_t)0x0;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) != 0) {
    RT_CALLRA(0x264258, guest::SoundStopAllChannels());
    RT_CALLRA(0x264260, guest::uiFade(0xffffffff80000000));
    iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x18)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) | 8;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 4))) = 0xffffffff;
    pvVar4 = (uint32_t)RT_CALLRA(0x264288, guest::gs_tasks_kill(param_1));
  }
  return pvVar4;
}


RT_GUEST_FN(0x00263e68, guest::distanceGoalRaceTask)

}  // namespace guest
