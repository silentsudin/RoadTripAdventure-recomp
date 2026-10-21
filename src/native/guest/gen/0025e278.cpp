// guest:0x0025e278 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t balloonCourseTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x28, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x38, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0x40, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x48) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4c) = 0x00000000u;
    rtguest::rtSpillF(rt_fg, 5, rt_sv, 0x58);
    rtguest::rtSpillF(rt_fg, 4, rt_sv, 0x50);
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
  }
  short sVar1;
  int iVar2;
  undefined4 uVar3;
uint32_t __s;
  int iVar4;
  uint uVar5;
  undefined8 uVar6;
  long lVar7;
uint32_t puVar8;
  int iVar9;
  uint uVar10;
  long lVar11;
  int iVar12;
  undefined4 uVar13;
  undefined4 uVar14;
  
  uVar3 = (*(uint32_t*)rtguest::G(0x333118));
  iVar9 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8))) == 0) {
    __s = (uint32_t)RT_CALLRA(0x25e2bc, guest::getExtraPacketArea(0x90c0));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28))) = __s;
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x90c0));
    RT_CALLRA(0x25e2d8, guest::InitObjectType21((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28)))));
    RT_CALLRA(0x25e2e4, guest::gs_music_play_bgm(0xa100201));
    RT_CALLRA(0x25e2f8, guest::gs_tasks_spawn(param_1,0x2357c0,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28)))));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28))) + 0x90b4))) = 0;
  }
  else {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8))) == 1) {
      (*(int32_t*)rtguest::G(0x177f5c8)) = (*(uint32_t*)rtguest::G(0x177f580));
      (*(uint64_t*)rtguest::G(0x177ea48)) = (*(uint64_t*)rtguest::G(0x177ea30));
      (*(int32_t*)rtguest::G(0x177f598)) = (*(uint32_t*)rtguest::G(0x177f580));
      (*(int32_t*)rtguest::G(0x177f5b0)) = (*(uint32_t*)rtguest::G(0x177f580));
      (*(uint64_t*)rtguest::G(0x177ea38)) = (*(uint64_t*)rtguest::G(0x177ea30));
      (*(uint64_t*)rtguest::G(0x177ea40)) = (*(uint64_t*)rtguest::G(0x177ea30));
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28)));
    }
    else {
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28)));
    }
    iVar12 = iVar2;
    switch((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x90b4)))) {
    case 0:
      uVar10 = 0;
      iVar12 = 0x370;
      lVar11 = 0;
      uVar5 = 0;
      uVar14 = (*(uint32_t*)rtguest::G(0x333114));
      puVar8 = (uint32_t)(iVar2 + 0x8cb0);
      do {
        lVar7 = (*(int64_t*)rtguest::G((uint32_t)param_2)) + lVar11;
        lVar11 = lVar11 + 1;
        uVar6 = RT_CALLRA(0x25e3d0, guest::__muldi3((*(uint32_t*)rtguest::G((uint32_t)0x2c4be0 + (uint32_t)(uVar5) * 4)),lVar7));
        iVar4 = RT_CALLRA(0x25e3dc, guest::__umoddi3(uVar6,5));
        uVar13 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002c4bf0 + iVar4 * 4)));
        (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(puVar8 + 6))) = (char)uVar10;
        uVar10 = uVar10 + 1;
        (*(uint32_t*)rtguest::G((uint32_t)puVar8 + (uint32_t)(2) * sizeof(uint32_t))) = rtguest::f2bits((float)iVar12);  // swc1 of cvt.s.w result (disasm 0x25e3e4-0x25e41c; fn:68) — bits, not value
        (*(uint32_t*)rtguest::G((uint32_t)puVar8)) = uVar13;
        iVar12 = iVar12 + 0x13;
        (*(uint32_t*)rtguest::G((uint32_t)puVar8 + (uint32_t)(1) * sizeof(uint32_t))) = uVar14;
        (*(uint32_t*)rtguest::G((uint32_t)puVar8 + (uint32_t)(4) * sizeof(uint32_t))) = uVar3;
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar8 + 0x19))) = 0;
        RT_CALLRA(0x25e438, guest::gs_tasks_spawn(param_1,0x25df80,puVar8));
        uVar5 = uVar10 & 0xf;
        puVar8 = puVar8 + 0x20;  // T* element step
      } while ((int)uVar10 < 0x20);
      RT_CALLRA(0x25e44c, guest::SelectMeshIntersectionRoutine(1));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x90b4))) = 1;
    case 1:
      if ((*(float*)rtguest::G(0x33311c)) <= (*(float*)rtguest::G(0x177ace8))) {
        RT_CALLRA(0x25e480, guest::SetRoadData((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28))),0,1));
        RT_CALLRA(0x25e488, guest::GetOrSetPresidentRaceResult(1));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x21))) = 0xd;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x90b4))) = 2;
        rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x334e88u)));
        iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28)));
      }
      else {
        if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 4))) == 1) {
          RT_CALLRA(0x25e4c4, guest::PlayUiSound(0x14));
          RT_CALLRA(0x25e4d4, guest::SetRoadData4((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28))),0,1));
          RT_CALLRA(0x25e4dc, guest::GetOrSetPresidentRaceResult(0));
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x90b4))) = 2;
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 40))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 40))) & 0xfffffffb;
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 4))) = 0;
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24))) = 0;
          rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x334e90u)));
        }
        RT_CALLRA(0x25e510, guest::raceHudDraw((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28)))));
        iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28)));
      }
      break;
    case 2:
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 40))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 40))) & 0xfffffeff;
      (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) | 0x800;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x90b4))) = 3;
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) = 0;
    case 3:
      sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2)));
      if (0x40 < (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2)))) {
        RT_CALLRA(0x25e564, guest::uiFade(((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) + -0x40) * 0x1000000));
        iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28)));
        sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2)));
      }
      if (sVar1 < 0xc0) {
        (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) = sVar1 + 1;
      }
      else {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x90b4))) = 0xffffffff;
      }
      break;
    case 0xffffffff:
    default:
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x90b8))) = 1;
    }
    RT_CALLRA(0x25e5a8, guest::raceHudDraw(iVar12));
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x90b8))) != 0) {
      RT_CALLRA(0x25e5bc, guest::SoundStopAllChannels());
      RT_CALLRA(0x25e5c4, guest::uiFade(0xffffffff80000000));
      iVar9 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x18)));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 40))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 40))) | 8;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 4))) = 0xffffffff;
      RT_CALLRA(0x25e5ec, guest::gs_tasks_kill(param_1));
    }
  }
  return 0;
}


// W13 fn/0025e278.c:95/105 `*(uint*)(param_2 + 5)` long*: typed-pointer arithmetic is element-scaled (x8); transliterator left it byte-unscaled
RT_GUEST_FN(0x0025e278, guest::balloonCourseTask)

}  // namespace guest
