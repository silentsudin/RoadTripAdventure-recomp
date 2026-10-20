// guest:0x00254430 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t TuningGarageSceneTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(96);
uint32_t __s;
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
uint32_t puVar5;
  uint uVar6;
uint32_t piVar7;
  uint uVar8;
uint32_t puVar9;
  int iVar10;
  uint uVar11;
  float fVar12;
  float fVar13;
  
  uVar11 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) & 1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) == 0) {
    __s = (uint32_t)getExtraPacketArea(0x8d70);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20))) = __s;
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x8d70));
    gs_music_play_bgm(0xa0d0201);
  }
  iVar10 = (int)param_2;
  uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28)));
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20)));
  puVar9 = (uint32_t)(iVar2 + 0x8cb0);
  if ((uVar4 & 0x8000) != 0) {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cbc))) == 0) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cbc))) = 1;
      SetBaseValue(iVar2,1);
      uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28)));
    }
    goto LAB_00254c0c;
  }
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cbc))) != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cbc))) = 0;
    SetBaseValue(iVar2,0);
  }
  switch((*(uint32_t*)rtguest::G((uint32_t)puVar9))) {
  case 0:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) & 0xfffffffb | 0x10000;
    puVar5 = 0x177ac50;
    uVar4 = 0;
    do {
      uVar6 = uVar4;
      if ((uVar11 != 0) && (uVar6 = (int)uVar4 / 2, (uVar4 & 1) != 0)) {
        uVar6 = (int)uVar4 / 2 + 3;
      }
      InitCameraTransforms(param_2,puVar5);
      iVar3 = uVar6 * 0x40;
      uVar8 = uVar4 + 1;
      StoreIndexedPosition
                ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x2ac490))),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x2ac494))),
                 (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x2ac498))),uVar4,(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x002ac480 + iVar3))));
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x24a))) = (char)uVar6;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0x4b))) = puVar9;
      puVar5 = puVar5 + 0x270;  // T* element step
      uVar4 = uVar8;
    } while ((int)uVar8 < 6);
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)((uint32_t)(iVar2 + 0x8cd0))), 0, 0x50));
    puVar5 = 0x177ac50;
    iVar3 = 5;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8ce8))) = 0x3f800000;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cf4))) = 0x40000000;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cf8))) = 0x43480000;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cd4))) = 0x3f800000;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cf0))) = 0x43480000;
    do {
      iVar3 = iVar3 + -1;
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0x33))) = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0x33))) | 0x800;
      puVar5 = puVar5 + 0x270;  // T* element step
    } while (-1 < iVar3);
    ResetMode1A(iVar2,uVar11);
    (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cb4))) = 0;
    goto LAB_0025464c;
  case 1:
LAB_0025464c:
    piVar7 = (uint32_t)(iVar2 + 0x8cb4);
    uiFade(0xffffffff80000000);
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar7));
    (*(int32_t*)rtguest::G((uint32_t)piVar7)) = iVar3 + 1;
    if (7 < iVar3) {
      (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 2;
      (*(int32_t*)rtguest::G((uint32_t)piVar7)) = 0;
LAB_00254688:
      piVar7 = (uint32_t)(iVar2 + 0x8cb4);
      uiFade((0x40 - (*(int32_t*)rtguest::G((uint32_t)piVar7))) * 0x2000000);
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar7));
      (*(int32_t*)rtguest::G((uint32_t)piVar7)) = iVar3 + 1;
      if (0x3f < iVar3) {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) & 0xfffeffff;
        SetHudState(iVar2,0,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cc4))),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cc8))),1);
        (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 3;
        (*(int32_t*)rtguest::G((uint32_t)piVar7)) = 0;
LAB_002546f8:
        piVar7 = (uint32_t)(iVar2 + 0x8cb4);
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar7));
        (*(int32_t*)rtguest::G((uint32_t)piVar7)) = iVar3 + 1;
        if (0x1f < iVar3) {
          SetHudState2(iVar2,1);
          PlayUiSound(0x30c);
          (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 4;
          (*(int32_t*)rtguest::G((uint32_t)piVar7)) = 0;
LAB_00254740:
          iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cb4)));
          (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cb4))) = iVar3 + 1;
          if (0x7f < iVar3) {
            puVar5 = 0x177ac50;
            iVar3 = 5;
            (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) | 0x104;
            do {
              iVar3 = iVar3 + -1;
              (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0x33))) = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0x33))) & 0xf7ff;
              puVar5 = puVar5 + 0x270;  // T* element step
            } while (-1 < iVar3);
            SetHudState2(iVar2,0);
            if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cc4))) + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cc8))) == 0) {
              PlayBgmSlot1();
            }
            (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cb4))) = 0;
            (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 5;
LAB_002547d4:
            iVar3 = 1;
            if (((*(float*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cf0))) <= 320.0) &&
               (iVar3 = 2, 80.0 <= (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cf0))))) {
              iVar3 = 0;
            }
            if (iVar3 != 0) {
              (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) & 0xfffffeff;
              PlayUiSound(0x30f);
              (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cc0))) = iVar3 + -1;
              piVar7 = (uint32_t)(iVar2 + 0x8cb4) + iVar3 + 3;
              (*(int32_t*)rtguest::G((uint32_t)piVar7)) = (*(int32_t*)rtguest::G((uint32_t)piVar7)) + 1;
              SetHudState3(iVar2,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cc0))),1);
              SetHudState(iVar2,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cc0))),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cc4))),
                          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cc8))),0);
              (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 6;
              (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cb4))) = 0;
LAB_00254898:
              iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cb4)));
              (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cb4))) = iVar3 + 1;
              if (0x7f < iVar3) {
                SetHudState3(iVar2,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cc0))),0);
                SetHudState(iVar2,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cc0))),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cc4))),
                            (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cc8))),2);
                uVar1 = 7;
LAB_00254994:
                (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = uVar1;
                (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cb4))) = 0;
              }
            }
          }
        }
      }
    }
    break;
  case 2:
    goto LAB_00254688;
  case 3:
    goto LAB_002546f8;
  case 4:
    goto LAB_00254740;
  case 5:
    goto LAB_002547d4;
  case 6:
    goto LAB_00254898;
  case 7:
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cb4)));
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cb4))) = iVar3 + 1;
    if (iVar3 < 0x80) break;
    SetHudState(iVar2,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cc0))),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cc4))),
                (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cc8))),0);
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cc4))) == 3) {
      SetWheelSprite(iVar2,0,1);
      GetOrSetPresidentRaceResult(2);
      if (uVar11 != 0) {
        SetWheelSprite2(iVar2,1,1);
      }
    }
    else {
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cc8))) != 3) {
        iVar3 = 5;
        puVar5 = 0x177ac50;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) | 0x10000;
        do {
          iVar3 = iVar3 + -1;
          (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0x33))) = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0x33))) | 0x800;
          puVar5 = puVar5 + 0x270;  // T* element step
        } while (-1 < iVar3);
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cb4))) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 8;
        break;
      }
      SetWheelSprite2(iVar2,0,1);
      GetOrSetPresidentRaceResult(1);
      if (uVar11 != 0) {
        SetWheelSprite(iVar2,1,1);
      }
    }
    uVar1 = 9;
    goto LAB_00254994;
  case 8:
    uiFade((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cb4))) << 0x18);
    piVar7 = (uint32_t)(iVar2 + 0x8cb4);
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar7));
    (*(int32_t*)rtguest::G((uint32_t)piVar7)) = iVar3 + 1;
    if (0x7f < iVar3) {
      (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 0;
      (*(int32_t*)rtguest::G((uint32_t)piVar7)) = 0;
    }
    break;
  case 9:
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cb4)));
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cb4))) = iVar3 + 1;
    if (0xff < iVar3) {
      SetWheelSprite(iVar2,0,0);
      SetWheelSprite2(iVar2,0,0);
      SetWheelSprite(iVar2,1,0);
      SetWheelSprite2(iVar2,1,0);
      iVar3 = 5;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) & 0xfffffeff | 0x10000;
      puVar5 = 0x177ac50;
      do {
        iVar3 = iVar3 + -1;
        (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0x33))) = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0x33))) | 0x800;
        puVar5 = puVar5 + 0x270;  // T* element step
      } while (-1 < iVar3);
      PlayBgmSlot5();
      (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 10;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cb4))) = 0;
    }
    break;
  case 10:
    piVar7 = (uint32_t)(iVar2 + 0x8cb4);
    uiFade((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cb4))) << 0x18);
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar7));
    (*(int32_t*)rtguest::G((uint32_t)piVar7)) = iVar3 + 1;
    if (0x7f < iVar3) {
      PlayBgmSlot2();
      (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 0xffffffff;
      (*(int32_t*)rtguest::G((uint32_t)piVar7)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cb8))) = 1;
    }
  }
  iVar3 = iVar2 + 0x8cd0;
  HandleCarObjectCollisions(iVar3,param_2);
  if (2.0 < (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cf4)))) {
    fVar12 = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8d08)));
    fVar13 = (*(float*)rtguest::G(0x333098));
  }
  else {
    fVar12 = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8d08)));
    fVar13 = (*(uint32_t*)rtguest::G(0x33309c));
  }
  (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8d00))) = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8d00))) * fVar13;
  (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8d08))) = fVar12 * fVar13;
  vu0AddVector(iVar2 + 0x8cf0,iVar2 + 0x8cf0,iVar2 + 0x8d00);
  ApplyArenaWallCollisions(iVar3);
  RotateObjectByWrappedYaw(iVar3);
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8d18))) != 0) {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8d18))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8d18))) + -1;
  }
  if (((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8d1c))) != 0) &&
     ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8d1c))) = 0, (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8d18))) == 0)) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8d18))) = 10;
    PlayUiSound(0x30a);
  }
  BuildTransformMatrix(0x177eb10,iVar3,iVar2 + 0x8ce0);
  vu0CopyVectorXYZ(0x177eb40,iVar2 + 0x8cf0);
  uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28)));
LAB_00254c0c:
  if ((uVar4 & 0x10000) == 0) {
    gs_map_draw_functions_race(param_2);
    raceMapMarkers(param_2);
    raceMapPickups(param_2,1);
    raceHudDraw(iVar2);
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cb8)));
  }
  else {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cb8)));
  }
  if (iVar2 == 0) {
    return 0;
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) | 8;
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x18)));
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24))) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = 0xffffffff;
  gs_tasks_kill();
  return 0;
}


RT_GUEST_FN(0x00254430, guest::TuningGarageSceneTask)

}  // namespace guest
