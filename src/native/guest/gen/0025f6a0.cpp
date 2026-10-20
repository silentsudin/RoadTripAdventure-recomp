// guest:0x0025f6a0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t finishZoneTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x48) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4c) = 0x00000000u;
  }
  short sVar1;
  undefined4 uVar2;
uint32_t __s;
  int iVar3;
  long lVar4;
  uint uVar5;
uint32_t puVar6;
  int iVar7;
  int iVar8;
uint32_t piVar9;
  int iVar10;
uint32_t puVar11;
  float fVar12;
  
  iVar10 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 8))) == 0) {
    __s = (uint32_t)RT_CALLRA(0x25f6e4, guest::getExtraPacketArea(0x8cc0));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) = __s;
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x8cc0));
    RT_CALLRA(0x25f700, guest::ResetMode12((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28)))));
    RT_CALLRA(0x25f70c, guest::gs_music_play_bgm(0xa100201));
    RT_CALLRA(0x25f720, guest::gs_tasks_spawn(param_1,0x2357c0,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28)))));
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb4))) = 0;
  }
  else {
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28)));
  }
  piVar9 = (uint32_t)(iVar3 + 0x8cb0);
  switch((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb4)))) {
  case 0:
    (*(int32_t*)rtguest::G((uint32_t)piVar9)) = 0xb4;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb4))) = 1;
  case 1:
    if ((((0.0 < ((*(uint32_t*)rtguest::G(0x2c4f5c)) - (*(float*)rtguest::G(0x2c4f54))) * ((*(uint32_t*)rtguest::G(0x177ace0)) - (*(uint32_t*)rtguest::G(0x2c4f58))) -
                 ((*(uint32_t*)rtguest::G(0x2c4f58)) - (*(uint32_t*)rtguest::G(0x2c4f50))) * ((*(uint32_t*)rtguest::G(0x177ace8)) - (*(uint32_t*)rtguest::G(0x2c4f5c)))) &&
         (0.0 < ((*(uint32_t*)rtguest::G(0x2c4f64)) - (*(uint32_t*)rtguest::G(0x2c4f5c))) * ((*(uint32_t*)rtguest::G(0x177ace0)) - (*(uint32_t*)rtguest::G(0x2c4f60))) -
                ((*(uint32_t*)rtguest::G(0x2c4f60)) - (*(uint32_t*)rtguest::G(0x2c4f58))) * ((*(uint32_t*)rtguest::G(0x177ace8)) - (*(uint32_t*)rtguest::G(0x2c4f64))))) &&
        (0.0 < ((*(uint32_t*)rtguest::G(0x2c4f6c)) - (*(uint32_t*)rtguest::G(0x2c4f64))) * ((*(uint32_t*)rtguest::G(0x177ace0)) - (*(uint32_t*)rtguest::G(0x2c4f68))) -
               ((*(uint32_t*)rtguest::G(0x2c4f68)) - (*(uint32_t*)rtguest::G(0x2c4f60))) * ((*(uint32_t*)rtguest::G(0x177ace8)) - (*(uint32_t*)rtguest::G(0x2c4f6c))))) &&
       (0.0 < ((*(float*)rtguest::G(0x2c4f54)) - (*(uint32_t*)rtguest::G(0x2c4f6c))) * ((*(uint32_t*)rtguest::G(0x177ace0)) - (*(uint32_t*)rtguest::G(0x2c4f50))) -
              ((*(uint32_t*)rtguest::G(0x2c4f50)) - (*(uint32_t*)rtguest::G(0x2c4f68))) * ((*(uint32_t*)rtguest::G(0x177ace8)) - (*(float*)rtguest::G(0x2c4f54))))) {
      RT_CALLRA(0x25f880, guest::PlayUiSound(0x14));
      RT_CALLRA(0x25f890, guest::SetRoadData((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))),0,1));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb4))) = 2;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0xfffffeff;
      (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) | 0x800;
      if (((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cbc))) < 0x2a31) && (lVar4 = TestStateBitFc78(0xe), lVar4 != 0)) {
        RT_CALLRA(0x25f8e0, guest::GetOrSetPresidentRaceResult(2));
      }
      else {
        RT_CALLRA(0x25f8f0, guest::GetOrSetPresidentRaceResult(1));
      }
    }
    if (44.0 <= (*(uint32_t*)rtguest::G(0x177ace0))) {
      if ((*(uint32_t*)rtguest::G(0x177ace0)) <= 56.0) {
        if ((*(uint32_t*)rtguest::G(0x177ace8)) <= 12.0) {
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb4))) = 3;
          (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) | 0x800;
          RT_CALLRA(0x25f96c, guest::GetOrSetPresidentRaceResult(0));
          uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28)));
        }
        else {
          uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28)));
        }
      }
      else {
        uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28)));
      }
    }
    else {
      uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28)));
    }
    if ((uVar5 & 4) == 0) {
LAB_0025f998:
      uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28)));
    }
    else {
      if ((uVar5 & 0x8000) == 0) {
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cbc))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cbc))) + 1;
        goto LAB_0025f998;
      }
      uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28)));
    }
    RT_CALLRA(0x25f99c, guest::InitHudConfig2(uVar2,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cbc)))));
    break;
  case 2:
    iVar7 = (*(int32_t*)rtguest::G((uint32_t)piVar9));
    (*(int32_t*)rtguest::G((uint32_t)piVar9)) = iVar7 + 1;
    if (iVar7 + 1 == 0xe1) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb4))) = 3;
    }
    break;
  case 3:
    uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28)));
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 2))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = uVar5 & 0xfffffeff;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb4))) = 4;
  case 4:
    sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 2)));
    if (0x40 < (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 2)))) {
      RT_CALLRA(0x25fa00, guest::uiFade(((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 2))) + -0x40) * 0x1000000));
      sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 2)));
    }
    if (sVar1 < 0xc0) {
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 2))) = sVar1 + 1;
    }
    else {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb4))) = 0xffffffff;
    }
    break;
  case 0xffffffff:
  default:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb8))) = 1;
  }
  iVar8 = 0;
  iVar7 = 0;
  fVar12 = (*(uint32_t*)rtguest::G(0x333134));
  puVar11 = 0x002c4f20;
  while( true ) {
    puVar6 = 0x0177eb10 + iVar7;
    RT_CALLRA(0x25fa6c, guest::vu0UnitMatrix(puVar6));
    RT_CALLRA(0x25fa7c, guest::vu0RotMatrixY((*(uint32_t*)rtguest::G(0x00333138)),puVar6,puVar6));
    if (iVar8 == 1) {
      RT_CALLRA(0x25facc, guest::vu0RotMatrixZ(((float)((*(int32_t*)rtguest::G((uint32_t)piVar9)) % 0x168 + -0xb4) * fVar12) / 180.0,0x177eb50,puVar6));
    }
    iVar8 = iVar8 + 1;
    RT_CALLRA(0x25fae8, guest::vu0CopyVector((int)0x0177eb40 + iVar7,puVar11));
    if (2 < iVar8) break;
    iVar7 = iVar8 * 0x40;
    puVar11 = puVar11 + 0x10;
  }
  RT_CALLRA(0x25fafc, guest::raceHudDraw((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28)))));
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb8))) != 0) {
    RT_CALLRA(0x25fb10, guest::SoundStopAllChannels());
    RT_CALLRA(0x25fb18, guest::uiFade(0xffffffff80000000));
    iVar10 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x18)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 0x400008;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 4))) = 0xffffffff;
    gs_tasks_kill(param_1);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x0025f6a0, guest::finishZoneTask)

}  // namespace guest
