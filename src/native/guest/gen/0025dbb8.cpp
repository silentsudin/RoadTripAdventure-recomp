// guest:0x0025dbb8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t stampCollectionTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  short sVar1;
  int iVar2;
  undefined2 uVar3;
uint32_t __s;
  undefined8 uVar4;
  long lVar5;
  ulong uVar6;
  int iVar7;
  int iVar8;
uint32_t pfVar9;
  int iVar10;
  uint uVar11;
uint32_t piVar12;
  long lVar13;
  uint uVar14;
  float fVar15;
  
  iVar10 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 8))) == 0) {
    __s = (uint32_t)getExtraPacketArea(0x9260);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) = __s;
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x9260));
    InitObjectType24((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))));
    gs_music_play_bgm(0xa100201);
    gs_tasks_spawn(param_1,0x2357c0,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))));
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x9254))) = 0;
  }
  else {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28)));
  }
  piVar12 = (uint32_t)(iVar2 + 0x9250);
  iVar8 = iVar2;
  switch((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x9254)))) {
  case 0:
    uVar14 = 0;
    SelectMeshIntersectionRoutine(1);
    lVar13 = 0;
    pfVar9 = (uint32_t)(iVar2 + 0x8cb0);
    do {
      uVar11 = uVar14 & 0xf;
      uVar4 = __muldi3((*(uint64_t*)rtguest::G((uint32_t)param_2)),lVar13);
      uVar4 = __muldi3(uVar4,(*(uint32_t*)rtguest::G((uint32_t)0x2c4bc0 + (uint32_t)(uVar11) * 4)));
      lVar5 = __umoddi3(uVar4,0xa0);
      uVar6 = lVar5 + 0x2d0;
      if ((long)uVar6 < 0) {
        fVar15 = (float)__floatdisf(uVar6 & 1 | uVar6 >> 1);
        (*(float*)rtguest::G((uint32_t)pfVar9)) = fVar15 + fVar15;
      }
      else {
        fVar15 = (float)__floatdisf();
        (*(float*)rtguest::G((uint32_t)pfVar9)) = fVar15;
      }
      uVar4 = __muldi3((*(uint64_t*)rtguest::G((uint32_t)param_2)),lVar13);
      uVar4 = __muldi3(uVar4,(*(uint32_t*)rtguest::G((uint32_t)0x2c4bc0 + (uint32_t)(uVar11) * 4)));
      lVar5 = __umoddi3(uVar4,0xa0);
      uVar6 = lVar5 + 0x2d0;
      if ((long)uVar6 < 0) {
        fVar15 = (float)__floatdisf(uVar6 & 1 | uVar6 >> 1);
        fVar15 = fVar15 + fVar15;
        (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)pfVar9 + 0x21))) = (char)uVar14;
      }
      else {
        fVar15 = (float)__floatdisf();
        (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)pfVar9 + 0x21))) = (char)uVar14;
      }
      uVar14 = uVar14 + 1;
      (*(float*)rtguest::G((uint32_t)pfVar9 + (uint32_t)(2) * sizeof(float))) = fVar15;
      uVar4 = __muldi3((*(uint64_t*)rtguest::G((uint32_t)param_2)),lVar13);
      uVar4 = __muldi3(uVar4,(*(uint32_t*)rtguest::G((uint32_t)0x2c4bc0 + (uint32_t)(uVar11) * 4)));
      uVar3 = __umoddi3(uVar4,0xffff);
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)pfVar9 + 0x2a))) = uVar3;
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(pfVar9 + 9))) = uVar3;
      gs_tasks_spawn(param_1,0x25d7c8,pfVar9);
      lVar13 = lVar13 + 1;
      pfVar9 = pfVar9 + 0x30;  // T* element step
    } while ((int)uVar14 < 0x1e);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x9254))) = 1;
  case 1:
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 4))) == 1) {
      (*(int32_t*)rtguest::G((uint32_t)piVar12)) = (*(int32_t*)rtguest::G((uint32_t)piVar12)) + 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 4))) = 0;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x24))) = 0;
      iVar7 = (*(int32_t*)rtguest::G((uint32_t)piVar12));
    }
    else {
      iVar7 = (*(int32_t*)rtguest::G((uint32_t)piVar12));
    }
    if (iVar7 == 0x14) {
      GetOrSetPresidentRaceResult(1);
      SetBumperConfig((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))),0,1);
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x9254))) = 2;
      iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28)));
      iVar7 = (*(int32_t*)rtguest::G((uint32_t)piVar12));
    }
    else {
      iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28)));
    }
    break;
  case 2:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 5))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 5))) & 0xfffffeff;
    (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) | 0x800;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x9254))) = 3;
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 2))) = 0;
    iVar7 = (*(int32_t*)rtguest::G((uint32_t)piVar12));
    break;
  case 3:
    sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 2)));
    if (0x40 < (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 2)))) {
      uiFade(((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 2))) + -0x40) * 0x1000000);
      iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28)));
      sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 2)));
    }
    if (sVar1 < 0xc0) {
      iVar7 = (*(int32_t*)rtguest::G((uint32_t)piVar12));
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 2))) = sVar1 + 1;
    }
    else {
      iVar7 = (*(int32_t*)rtguest::G((uint32_t)piVar12));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x9254))) = 0xffffffff;
    }
    break;
  case 0xffffffff:
  default:
    iVar7 = (*(int32_t*)rtguest::G((uint32_t)piVar12));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x9258))) = 1;
  }
  SetObjectFields848C8480(iVar8,iVar7,1);
  raceHudDraw((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))));
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x9258))) != 0) {
    SoundStopAllChannels();
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x9258))) = 1;
    uiFade(0xffffffff80000000);
    iVar10 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x18)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 5))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 5))) | 8;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 4))) = 0xffffffff;
    gs_tasks_kill(param_1);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x0025dbb8, guest::stampCollectionTask)

}  // namespace guest
