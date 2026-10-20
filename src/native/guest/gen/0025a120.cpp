// guest:0x0025a120 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t raceSceneTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(80);
  char cVar1;
  byte bVar2;
  short sVar3;
  int iVar4;
  undefined4 uVar5;
uint32_t pvVar6;
  int iVar7;
  uint uVar8;
uint32_t pcVar9;
  ulong uVar10;
uint32_t pbVar11;
  int iVar12;
  long lVar13;
  int iVar14;
uint32_t puVar15;
  int iVar16;
  
  bVar2 = (*(int8_t*)rtguest::G(0x177ae9a));
  iVar14 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 8))) == 0) {
    (*(uint64_t*)rtguest::G(0x177ea48)) = (*(uint64_t*)rtguest::G(0x177ea30));
    (*(int32_t*)rtguest::G(0x177f5c8)) = (*(uint32_t*)rtguest::G(0x177f580));
    (*(int8_t*)rtguest::G(0x177ae9a)) = 0;
    (*(uint64_t*)rtguest::G(0x177ea38)) = (*(uint64_t*)rtguest::G(0x177ea30));
    (*(uint64_t*)rtguest::G(0x177ea40)) = (*(uint64_t*)rtguest::G(0x177ea30));
    (*(int32_t*)rtguest::G(0x177f598)) = (*(uint32_t*)rtguest::G(0x177f580));
    (*(int32_t*)rtguest::G(0x177f5b0)) = (*(uint32_t*)rtguest::G(0x177f580));
    pvVar6 = (uint32_t)getExtraPacketArea(0x11230);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x28))) = pvVar6;
    pvVar6 = (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(pvVar6)), 0, 0x11230));
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x28))) + 0x90f0))) = 0;
    return pvVar6;
  }
  iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x28)));
  puVar15 = (uint32_t)(iVar4 + 0x90f0);
  iVar16 = (int)param_2;
  switch((*(uint16_t*)rtguest::G((uint32_t)puVar15))) {
  case 0:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x90f8))) = 0;
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 2))) = 0;
    (*(int8_t*)rtguest::G(0x177ae9a)) = 0;
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x28))))), 0, 0x11230));
    iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x28))) + 0x9100;
    initRaceStandings(iVar12,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x1c))));
    gs_tasks_spawn(param_1,0x25ab28,iVar12);
    SetGamePausedState(param_2,1);
    (*(uint16_t*)rtguest::G((uint32_t)puVar15)) = 5;
    InitCameraTransforms(param_2,0x177ac50);
    iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x90f4)));
    goto LAB_0025a86c;
  case 1:
    uVar10 = (uint32_t)(*(int8_t*)rtguest::G(0x177ae9a));
    iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x1c)));
    if ((uVar10 != 0) && (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar16 + 0x28))) & 0x8000) == 0)) {
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x90f8))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x90f8))) + 1;
    }
    if ((long)uVar10 < (long)(char)(*(uint32_t*)rtguest::G((uint32_t)0x2c3a38 + (uint32_t)(iVar12) * 4))) {
      pcVar9 = (*(uint32_t*)rtguest::G((uint32_t)(0x2c3238u + (uint32_t)(iVar12) * 4))) + (uint32_t)bVar2 * 6;
      iVar7 = (*(int8_t*)rtguest::G((uint32_t)pcVar9)) * 0x10;
      if (0.0 <= (((*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c325c + iVar7))) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c3254 + iVar7)))) *
                  ((*(uint32_t*)rtguest::G(0x177ace0)) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c3258 + iVar7)))) -
                 ((*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c3258 + iVar7))) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c3250 + iVar7)))) *
                 ((*(uint32_t*)rtguest::G(0x177ace8)) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c325c + iVar7))))) * (float)(int)(*(int8_t*)rtguest::G((uint32_t)pcVar9 + (uint32_t)(1) * sizeof(int8_t)))) {
        (*(int8_t*)rtguest::G(0x177ae9a)) = (*(int8_t*)rtguest::G(0x177ae9a)) + 1;
        cVar1 = (*(int8_t*)rtguest::G((uint32_t)pcVar9 + (uint32_t)(2) * sizeof(int8_t)));
      }
      else {
        cVar1 = (*(int8_t*)rtguest::G((uint32_t)pcVar9 + (uint32_t)(2) * sizeof(int8_t)));
      }
      iVar7 = cVar1 * 0x10;
      if ((((*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c325c + iVar7))) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c3254 + iVar7)))) *
           ((*(uint32_t*)rtguest::G(0x177ace0)) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c3258 + iVar7)))) -
          ((*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c3258 + iVar7))) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c3250 + iVar7)))) *
          ((*(uint32_t*)rtguest::G(0x177ace8)) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c325c + iVar7))))) * (float)(int)(*(int8_t*)rtguest::G((uint32_t)pcVar9 + (uint32_t)(3) * sizeof(int8_t))) < 0.0) {
        iVar7 = (*(int8_t*)rtguest::G((uint32_t)pcVar9 + (uint32_t)(4) * sizeof(int8_t))) * 0x10;
        if ((((*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c325c + iVar7))) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c3254 + iVar7)))) *
             ((*(uint32_t*)rtguest::G(0x177ace0)) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c3258 + iVar7)))) -
            ((*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c3258 + iVar7))) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c3250 + iVar7)))) *
            ((*(uint32_t*)rtguest::G(0x177ace8)) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c325c + iVar7))))) * (float)(int)(*(int8_t*)rtguest::G((uint32_t)pcVar9 + (uint32_t)(5) * sizeof(int8_t))) < 0.0)
        goto LAB_0025a5c0;
        uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x28)));
      }
      else {
        uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x28)));
      }
      SetObjectState(uVar5,0,1);
      (*(uint16_t*)rtguest::G((uint32_t)puVar15)) = 2;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x90f8))) = 0;
      GetOrSetPresidentRaceResult(0);
      iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x90f4)));
    }
    else {
LAB_0025a5c0:
      if ((uVar10 != 0) &&
         (iVar7 = (char)(*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(0x2c3238u + (uint32_t)(iVar12) * 4))) + (uint32_t)((uint32_t)bVar2 * 6 + -6) * 4)) * 0x10,
         (((*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c325c + iVar7))) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c3254 + iVar7)))) *
          ((*(uint32_t*)rtguest::G(0x177ace0)) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c3258 + iVar7)))) -
         ((*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c3258 + iVar7))) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c3250 + iVar7)))) *
         ((*(uint32_t*)rtguest::G(0x177ace8)) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c325c + iVar7))))) *
         (float)(int)(char)(*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(0x2c3238u + (uint32_t)(iVar12) * 4))) + (uint32_t)((uint32_t)bVar2 * 6 + -5) * 4)) < 0.0)) {
        (*(int8_t*)rtguest::G(0x177ae9a)) = (*(int8_t*)rtguest::G(0x177ae9a)) - 1;
      }
      if ((long)(char)(*(uint32_t*)rtguest::G((uint32_t)0x2c3a38 + (uint32_t)(iVar12) * 4)) == uVar10) {
        GetOrSetPresidentRaceResult(1);
        if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x90fc))) == 0) {
          gs_stamps_award_function((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x2c3a40u + (uint32_t)(iVar12) * 4)));
          uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x28)));
        }
        else {
          uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x28)));
        }
        SetBumperConfig(uVar5,0,1);
        (*(uint16_t*)rtguest::G((uint32_t)puVar15)) = 2;
        iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 4)));
      }
      else {
        iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 4)));
      }
      if (iVar12 == 1) {
        PlayUiSound(0x314);
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x90fc))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x90fc))) + 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 4))) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x90f4))) = 0x80;
        iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 4)));
      }
      if (iVar12 == 2) {
        PlayUiSound(0x314);
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 4))) = 0;
        uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x28)));
      }
      else {
        uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x28)));
      }
      InitHudConfig2(uVar5,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x90f8))));
      iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x90f4)));
    }
    goto LAB_0025a86c;
  case 2:
    (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) | 0x800;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar16 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar16 + 0x28))) & 0xfffffeff | 8;
    (*(uint16_t*)rtguest::G((uint32_t)puVar15)) = 3;
    PlayBgmSlot5();
    iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x90f4)));
    goto LAB_0025a86c;
  case 3:
    uiFade((int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 2))) << 0x18);
    if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 2))) < 0x80) {
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 2))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 2))) + 1;
    }
    else {
      iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x28))) + 0x9100;
      storeRacerResult(iVar12,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x90f8))),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x90fc))));
      gs_tasks_spawn(param_1,0x25b0c0,iVar12);
      gs_music_play_bgm(0xa100201);
      SetObjectState((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x28))),0,0);
      SetBumperConfig((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x28))),0,0);
      SetGamePausedState(param_2,1);
      uVar8 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar16 + 0x28)));
      (*(uint16_t*)rtguest::G((uint32_t)puVar15)) = 4;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar16 + 0x28))) = uVar8 | 0x10000;
    }
    break;
  case 4:
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x24))) != '\0') {
      (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) & 0xf7ff;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar16 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar16 + 0x28))) & 0xfffffff3;
      (*(uint16_t*)rtguest::G((uint32_t)puVar15)) = 0;
    }
    break;
  case 5:
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x24))) != '\0') {
      (*(uint32_t*)rtguest::G(0x177ae8c)) = 0x40000;
      iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 4)));
      if (iVar12 == -1) goto switchD_0025a200_caseD_ffff;
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x1c))) = iVar12;
      lVar13 = 0;
      StoreIndexedPosition
                ((float)(int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 * 4 + 0x2c3a18))),0x3f800000,
                 (float)(int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(0x002c3a1a + iVar12 * 4))),0,0);
      pbVar11 = (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x28))) + 0x8cb0);
      DestroyTaskRecord(param_1);
      if ('\0' < (char)(*(uint32_t*)rtguest::G((uint32_t)0x2c3a30 + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x1c)))) * 4))) {
        (*(int8_t*)rtguest::G((uint32_t)pbVar11)) = 0;
        while( true ) {
          iVar12 = (int)lVar13 + 1;
          lVar13 = (long)iVar12;
          bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x1c)));
          (*(int8_t*)rtguest::G((uint32_t)pbVar11 + (uint32_t)(1) * sizeof(int8_t))) = bVar2;
          vu0CopyVector(pbVar11 + 0x20,(*(uint32_t*)rtguest::G((uint32_t)(0x2c3a00u + (uint32_t)(bVar2) * 4))) + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar11)) * 0x10);
          gs_tasks_spawn(param_1,0x259a08,pbVar11);
          if ((char)(*(uint32_t*)rtguest::G((uint32_t)0x2c3a30 + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x1c)))) * 4)) <= lVar13) break;
          (*(int8_t*)rtguest::G((uint32_t)pbVar11 + (uint32_t)(0x40) * sizeof(int8_t))) = (uint32_t)iVar12;
          pbVar11 = pbVar11 + 0x40;
        }
      }
      ResetMode19((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x28))));
      gs_music_play_bgm(0xa100201);
      gs_tasks_spawn(param_1,0x2357c0,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x28))));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 4))) = 0;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x24))) = 0;
      SetGamePausedState(param_2,0);
      uVar8 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar16 + 0x28)));
      (*(uint16_t*)rtguest::G((uint32_t)puVar15)) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar16 + 0x28))) = uVar8 & 0xfffeffff;
    }
    break;
  case 0xffff:
switchD_0025a200_caseD_ffff:
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x90f2))) = 1;
    break;
  default:
    iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x90f4)));
    goto LAB_0025a86c;
  }
  iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x90f4)));
LAB_0025a86c:
  uVar8 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar16 + 0x28)));
  if ((0 < iVar12) && ((uVar8 & 0x8000) == 0)) {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x90f4))) = iVar12 + -1;
    uVar8 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar16 + 0x28)));
  }
  if ((uVar8 & 0x10000) == 0) {
    gs_map_draw_functions_scissored(param_2,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x1c))));
    raceMapMarkers(param_2);
    raceMapPickups(param_2,(*(uint32_t*)rtguest::G((uint32_t)0x2c3a30 + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x1c)))) * 4)));
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x90f4))) & 0x10) == 0) {
      SetFlagConfig((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x28))),0,0);
    }
    else {
      SetFlagConfig((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x28))),0,1);
    }
    raceHudDraw((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x28))));
    sVar3 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x90f2)));
  }
  else {
    sVar3 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x90f2)));
  }
  pvVar6 = (uint32_t)0x0;
  if (sVar3 != 0) {
    SoundStopAllChannels();
    uiFade(0xffffffff80000000);
    iVar14 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x18)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar16 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar16 + 0x28))) | 8;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 4))) = 0xffffffff;
    pvVar6 = (uint32_t)gs_tasks_kill(param_1);
  }
  return pvVar6;
}


RT_GUEST_FN(0x0025a120, guest::raceSceneTask)

}  // namespace guest
