// guest:0x0025e630 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t goalPlaneTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
  }
  short sVar1;
  int iVar2;
  uint uVar3;
uint32_t __s;
  int iVar4;
  long lVar5;
  undefined4 uVar6;
uint32_t piVar7;
  int iVar8;
  
  iVar8 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 8))) == 0) {
    __s = (uint32_t)RT_CALLRA(0x25e660, guest::getExtraPacketArea(0x8cc0));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))) = __s;
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x8cc0));
    RT_CALLRA(0x25e67c, guest::InitObjectType1F((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)))));
    RT_CALLRA(0x25e688, guest::gs_music_play_bgm(0xa100201));
    RT_CALLRA(0x25e69c, guest::gs_tasks_spawn(param_1,0x2357c0,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)))));
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb4))) = 0;
  }
  else {
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
  }
  piVar7 = (uint32_t)(iVar4 + 0x8cb0);
  switch((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb4)))) {
  case 0:
    (*(int32_t*)rtguest::G((uint32_t)piVar7)) = 0xb4;
    (*(float*)rtguest::G(0x2c4c34)) = 170.0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb4))) = 1;
    goto LAB_0025e724;
  case 1:
LAB_0025e724:
    if ((((169.0 <= (*(float*)rtguest::G(0x177ace4))) && ((*(float*)rtguest::G(0x177ace4)) < 171.0)) &&
        (0.0 < ((*(float*)rtguest::G(0x2c4c4c)) - (*(float*)rtguest::G(0x2c4c44))) * ((*(float*)rtguest::G(0x177ace0)) - (*(float*)rtguest::G(0x2c4c48))) -
               ((*(float*)rtguest::G(0x2c4c48)) - (*(float*)rtguest::G(0x2c4c40))) * ((*(float*)rtguest::G(0x177ace8)) - (*(float*)rtguest::G(0x2c4c4c))))) &&
       (((0.0 < ((*(float*)rtguest::G(0x2c4c54)) - (*(float*)rtguest::G(0x2c4c4c))) * ((*(float*)rtguest::G(0x177ace0)) - (*(float*)rtguest::G(0x2c4c50))) -
                ((*(float*)rtguest::G(0x2c4c50)) - (*(float*)rtguest::G(0x2c4c48))) * ((*(float*)rtguest::G(0x177ace8)) - (*(float*)rtguest::G(0x2c4c54))) &&
         (0.0 < ((*(float*)rtguest::G(0x2c4c5c)) - (*(float*)rtguest::G(0x2c4c54))) * ((*(float*)rtguest::G(0x177ace0)) - (*(float*)rtguest::G(0x2c4c58))) -
                ((*(float*)rtguest::G(0x2c4c58)) - (*(float*)rtguest::G(0x2c4c50))) * ((*(float*)rtguest::G(0x177ace8)) - (*(float*)rtguest::G(0x2c4c5c))))) &&
        (0.0 < ((*(float*)rtguest::G(0x2c4c44)) - (*(float*)rtguest::G(0x2c4c5c))) * ((*(float*)rtguest::G(0x177ace0)) - (*(float*)rtguest::G(0x2c4c40))) -
               ((*(float*)rtguest::G(0x2c4c40)) - (*(float*)rtguest::G(0x2c4c58))) * ((*(float*)rtguest::G(0x177ace8)) - (*(float*)rtguest::G(0x2c4c44))))))) {
      RT_CALLRA(0x25e84c, guest::PlayUiSound(0x14));
      (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) | 0x800;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0xfffffefb;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb4))) = 2;
      lVar5 = RT_CALLRA(0x25e878, guest::TestStateBitFc78(0x37));
      if (lVar5 == 0) {
        RT_CALLRA(0x25e898, guest::GetOrSetPresidentRaceResult(1));
        uVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
      }
      else if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cbc))) < 0x1069) {
        RT_CALLRA(0x25e8a8, guest::GetOrSetPresidentRaceResult(2));
        uVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
      }
      else {
        RT_CALLRA(0x25e8b8, guest::GetOrSetPresidentRaceResult(0));
        uVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
      }
      RT_CALLRA(0x25e8c8, guest::SetRoadData(uVar6,0,1));
    }
    if ((*(float*)rtguest::G(0x177ace4)) <= 100.0) {
      RT_CALLRA(0x25e8f8, guest::SetRoadData4((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))),0,1));
      RT_CALLRA(0x25e900, guest::GetOrSetPresidentRaceResult(0));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb4))) = 3;
      (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) | 0x800;
    }
    if ((*(float*)rtguest::G(0x177ace0)) <= 110.0) {
      RT_CALLRA(0x25e93c, guest::GetOrSetPresidentRaceResult(0));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb4))) = 3;
      (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) | 0x800;
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28)));
    }
    else {
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28)));
    }
    if ((uVar3 & 4) == 0) {
LAB_0025e97c:
      uVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
    }
    else {
      if ((uVar3 & 0x8000) == 0) {
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cbc))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cbc))) + 1;
        goto LAB_0025e97c;
      }
      uVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
    }
    RT_CALLRA(0x25e980, guest::InitHudConfig2(uVar6,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cbc)))));
    break;
  case 2:
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)piVar7));
    (*(float*)rtguest::G(0x2c4c34)) = (*(float*)rtguest::G(0x2c4c34)) + (*(float*)rtguest::G(0x333120));
    (*(int32_t*)rtguest::G((uint32_t)piVar7)) = iVar2 + 1;
    if (iVar2 + 1 == 0xe1) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb4))) = 3;
    }
    break;
  case 3:
    uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28)));
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = uVar3 & 0xfffffefb;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb4))) = 4;
  case 4:
    sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2)));
    if (0x40 < (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2)))) {
      RT_CALLRA(0x25e9f8, guest::uiFade(((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))) + -0x40) * 0x1000000));
      sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2)));
    }
    if (sVar1 < 0xc0) {
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))) = sVar1 + 2;
    }
    else {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb4))) = 0xffffffff;
    }
    break;
  case 0xffffffff:
  default:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb8))) = 1;
  }
  RT_CALLRA(0x25ea54, guest::vu0UnitMatrix(0x177eb10));
  RT_CALLRA(0x25ea60, guest::vu0CopyVector(0x177eb40,0x2c4c10));
  RT_CALLRA(0x25ea68, guest::vu0UnitMatrix(0x177eb50));
  RT_CALLRA(0x25eabc, guest::vu0RotMatrixX(((float)((*(int32_t*)rtguest::G((uint32_t)piVar7)) % 0x168 + -0xb4) * (*(float*)rtguest::G(0x333124))) / 180.0,0x177eb50,0x177eb50));
  RT_CALLRA(0x25eac8, guest::vu0CopyVector(0x177eb80,0x2c4c20));
  RT_CALLRA(0x25ead0, guest::vu0UnitMatrix(0x177eb90));
  RT_CALLRA(0x25eadc, guest::vu0CopyVector(0x177ebc0,0x2c4c30));
  RT_CALLRA(0x25eae4, guest::raceHudDraw((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)))));
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb8))) == 0) {
    return 0;
  }
  RT_CALLRA(0x25eaf8, guest::SoundStopAllChannels());
  RT_CALLRA(0x25eb00, guest::uiFade(0xffffffff80000000));
  iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x18)));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 8;
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x24))) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 4))) = 0xffffffff;
  gs_tasks_kill(param_1);
  return 0;
}


RT_GUEST_FN(0x0025e630, guest::goalPlaneTask)

}  // namespace guest
