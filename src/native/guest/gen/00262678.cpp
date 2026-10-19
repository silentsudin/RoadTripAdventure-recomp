// guest:0x00262678 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t texAnimCopyB(uint32_t param_1, uint32_t param_2, ...) {
  byte bVar1;
  short sVar2;
  int iVar3;
  float fVar4;
  float fVar5;
  undefined1 uVar6;
  short sVar7;
  int iVar8;
  undefined4 uVar9;
uint32_t pvVar10;
  undefined8 uVar11;
  uint uVar12;
  int iVar13;
  int iVar14;
uint32_t psVar15;
  int iVar16;
  int iVar17;
uint32_t puVar18;
  
  iVar17 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 8))) == 0) {
    uVar11 = getExtraPacketArea(0x8cc4);
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x28))) = (int)uVar11;
    ResetMode1E(uVar11);
    gs_music_play_bgm(0xa100201);
    iVar17 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x28)));
    puVar18 = (uint32_t)(iVar17 + 0x8cb0);
    pvVar10 = (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(puVar18)), 0, 0x14));
    (*(uint32_t*)rtguest::G((uint32_t)puVar18)) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x8cbd))) = 0;
    return pvVar10;
  }
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x28)));
  puVar18 = (uint32_t)(iVar3 + 0x8cb0);
  iVar8 = iVar3;
  switch((*(uint32_t*)rtguest::G((uint32_t)puVar18))) {
  case 0:
    gs_tasks_spawn(param_1,0x2357c0);
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x26))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)puVar18)) = 1;
  case 1:
    uVar6 = __umoddi3((*(uint64_t*)rtguest::G((uint32_t)param_2)) >> 1,3);
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cbe))) = uVar6;
    uVar9 = (*(uint32_t*)rtguest::G(0x177ad48));
    InitCameraTransforms(param_2,0x177ac50);
    BgmStop1();
    iVar8 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cbe))) * 0x10;
    StoreIndexedPosition
              ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x2d9e70))),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x2d9e74))),
               (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002d9e78 + iVar8))),0,0);
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cbf)));
    (*(uint32_t*)rtguest::G(0x177ad48)) = uVar9;
    uVar6 = __umoddi3((*(uint64_t*)rtguest::G((uint32_t)param_2)),(*(uint32_t*)rtguest::G((uint32_t)0x2d9f1c + (uint32_t)((uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x26))) * 8) * 4)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cbf))) = uVar6;
    uVar6 = __umoddi3((*(uint64_t*)rtguest::G((uint32_t)param_2)) >> 8,(*(uint32_t*)rtguest::G((uint32_t)0x2d9f1c + (uint32_t)((uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x26))) * 8) * 4)));
    uVar12 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cbf)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cc0))) = uVar6;
    if (bVar1 == uVar12) {
      uVar12 = bVar1 + 2;
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cbf))) = (char)uVar12;
      uVar12 = uVar12 & 0xff;
    }
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cbf))) == (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cc0)))) {
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cc0))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cc0))) + '\x02';
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x26)));
    }
    else {
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x26)));
    }
    if ((*(uint32_t*)rtguest::G((uint32_t)0x2d9f1c + (uint32_t)((uint32_t)bVar1 * 8) * 4)) == 0) {
      trap(7);
    }
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cbf))) = (char)(uVar12 % (uint32_t)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x2d9f1c + (uint32_t)((uint32_t)bVar1 * 8) * 4)));
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cc0))) =
         (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cc0))) % (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x2d9f1c + (uint32_t)((uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x26))) * 8) * 4));
    uVar6 = 1;
    if (((*(uint64_t*)rtguest::G((uint32_t)param_2)) & 1) == 0) {
      uVar6 = 0xff;
    }
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cbc))) = uVar6;
    (*(uint32_t*)rtguest::G((uint32_t)puVar18)) = 2;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb8))) = (int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(0x002d9f40 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cbe))) * 2)));
LAB_002628dc:
    fVar5 = (*(uint32_t*)rtguest::G(0x177ace8));
    fVar4 = (*(uint32_t*)rtguest::G(0x177ace0));
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cbd)));
    if (bVar1 == 1) {
      if (432.0 <= (*(uint32_t*)rtguest::G(0x177ace8))) {
        SetRoadData((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x28))),0,1);
LAB_00262b5c:
        (*(uint32_t*)rtguest::G((uint32_t)puVar18)) = 3;
        uVar12 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x26)));
      }
      else {
        uVar12 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x26)));
      }
    }
    else if (bVar1 < 2) {
      if (bVar1 == 0) {
        iVar8 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cbe))) * 0x10;
        if (0.0 <= ((*(float*)rtguest::G((uint32_t)(uint32_t)(0x002d9e4c + iVar8))) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002d9e44 + iVar8)))) *
                   ((*(uint32_t*)rtguest::G(0x177ace0)) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002d9e48 + iVar8)))) -
                   ((*(float*)rtguest::G((uint32_t)(uint32_t)(0x002d9e48 + iVar8))) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002d9e40 + iVar8)))) *
                   ((*(uint32_t*)rtguest::G(0x177ace8)) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002d9e4c + iVar8))))) {
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 5))) = (uint32_t)(*(uint64_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(5) * sizeof(uint64_t))) & 0xfffffeff;
          iVar8 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cbe))) * 0x10;
          if (0.0 <= (((*(float*)rtguest::G((uint32_t)(uint32_t)(0x002d9e1c + iVar8))) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002d9e14 + iVar8)))) *
                      (fVar4 - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002d9e18 + iVar8)))) -
                     ((*(float*)rtguest::G((uint32_t)(uint32_t)(0x002d9e18 + iVar8))) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002d9e10 + iVar8)))) *
                     (fVar5 - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002d9e1c + iVar8))))) *
                     (float)(int)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cbc)))) {
            if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x26))) < 4) {
              (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cbd))) = 0;
            }
            else {
              GetOrSetPresidentRaceResult(1);
              (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cbd))) = 1;
            }
          }
          else {
            GetOrSetPresidentRaceResult(0);
            (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cbd))) = 2;
          }
          (*(uint32_t*)rtguest::G((uint32_t)puVar18)) = 5;
        }
        if (((*(uint64_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(5) * sizeof(uint64_t))) & 0x8004) == 4) {
          (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb8))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb8))) + -1;
          iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb8)));
        }
        else {
          iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb8)));
        }
        if (iVar8 == 0) {
          uVar9 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x28)));
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 5))) = (uint32_t)(*(uint64_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(5) * sizeof(uint64_t))) & 0xfffffeff;
          SetRoadData6(uVar9,0,1);
          (*(uint32_t*)rtguest::G((uint32_t)puVar18)) = 3;
          GetOrSetPresidentRaceResult(0);
          iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb8)));
        }
        SetTrackData((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x28))),iVar8 / 0x3c,1);
        uVar12 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x26)));
      }
      else {
        uVar12 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x26)));
      }
    }
    else if (bVar1 == 2) {
      SetRoadData4((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x28))),0,1);
      (*(uint32_t*)rtguest::G(0x177ad48)) = 0x23a0;
      (*(uint32_t*)rtguest::G(0x177ad40)) = 0;
      if (((*(uint32_t*)rtguest::G(0x177ace4)) <= 80.0) || (100.0 <= (*(uint32_t*)rtguest::G(0x177ace8)))) goto LAB_00262b5c;
      uVar12 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x26)));
    }
    else {
      uVar12 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x26)));
    }
    psVar15 = (uint32_t)((*(uint32_t*)rtguest::G((uint32_t)(0x2d9f18u + (uint32_t)(uVar12 * 2) * 4))) + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cbf))) * 4);
    sVar7 = (*(int16_t*)rtguest::G((uint32_t)psVar15 + (uint32_t)(1) * sizeof(int16_t)));
    sVar2 = (*(int16_t*)rtguest::G((uint32_t)psVar15));
    iVar8 = (int)sVar7 * (int)(*(uint32_t*)rtguest::G(0x2da04a));
    iVar13 = (int)sVar2 * (int)(*(uint32_t*)rtguest::G(0x2da048));
    if (iVar8 < 0) {
      iVar8 = iVar8 + 0xf;
    }
    if (iVar13 < 0) {
      iVar13 = iVar13 + 0xf;
    }
    (*(uint32_t*)rtguest::G(0x2d9f80)) = (long)(iVar13 >> 4) & (uint32_t)(*(uint32_t*)rtguest::G(0x2da04c)) |
                   ((long)(iVar8 >> 4) & (uint32_t)(*(uint32_t*)rtguest::G(0x2da04e))) << 0x10;
    (*(uint32_t*)rtguest::G(0x2d9f70)) = (long)(*(uint32_t*)rtguest::G(0x2da030)) | (long)(*(uint32_t*)rtguest::G(0x2da03c)) << 0x38 |
                   (long)(*(uint32_t*)rtguest::G(0x2da034)) << 0x20 | (long)(*(uint32_t*)rtguest::G(0x2da03c)) << 0x18 |
                   (long)(*(uint32_t*)rtguest::G(0x2da038)) << 0x10 | (long)(*(uint32_t*)rtguest::G(0x2da038)) << 0x30;
    (*(uint32_t*)rtguest::G(0x2d9f90)) = (long)(*(uint32_t*)rtguest::G(0x2da040)) | (long)(*(uint32_t*)rtguest::G(0x2da044)) << 0x20;
    psVar15 = (uint32_t)((*(uint32_t*)rtguest::G((uint32_t)(0x2d9f18u + (uint32_t)(uVar12 * 2) * 4))) + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cc0))) * 4);
    iVar8 = (int)(*(int16_t*)rtguest::G((uint32_t)psVar15 + (uint32_t)(1) * sizeof(int16_t)));
    iVar13 = (int)(*(int16_t*)rtguest::G((uint32_t)psVar15));
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cbc))) < '\x01') {
      iVar14 = (int)sVar7 * (int)(*(uint32_t*)rtguest::G(0x2da06a));
      iVar8 = iVar8 * (*(uint32_t*)rtguest::G(0x2da08a));
      iVar16 = (int)sVar2 * (int)(*(uint32_t*)rtguest::G(0x2da068));
      iVar13 = iVar13 * (*(uint32_t*)rtguest::G(0x2da088));
      if (iVar14 < 0) {
        iVar14 = iVar14 + 0xf;
      }
      if (iVar8 < 0) {
        iVar8 = iVar8 + 0xf;
      }
      if (iVar16 < 0) {
        iVar16 = iVar16 + 0xf;
      }
      if (iVar13 < 0) {
        iVar13 = iVar13 + 0xf;
      }
      (*(uint32_t*)rtguest::G(0x2d9fc0)) = (long)(iVar16 >> 4) & (uint32_t)(*(uint32_t*)rtguest::G(0x2da06c)) |
                     ((long)(iVar14 >> 4) & (uint32_t)(*(uint32_t*)rtguest::G(0x2da06e))) << 0x10;
      (*(uint32_t*)rtguest::G(0x2da000)) = (long)(iVar13 >> 4) & (uint32_t)(*(uint32_t*)rtguest::G(0x2da08c)) |
                     ((long)(iVar8 >> 4) & (uint32_t)(*(uint32_t*)rtguest::G(0x2da08e))) << 0x10;
      (*(uint32_t*)rtguest::G(0x2d9fb0)) = (long)(*(uint32_t*)rtguest::G(0x2da050)) | (long)(*(uint32_t*)rtguest::G(0x2da05c)) << 0x38 |
                     (long)(*(uint32_t*)rtguest::G(0x2da054)) << 0x20 | (long)(*(uint32_t*)rtguest::G(0x2da05c)) << 0x18 |
                     (long)(*(uint32_t*)rtguest::G(0x2da058)) << 0x10 | (long)(*(uint32_t*)rtguest::G(0x2da058)) << 0x30;
      (*(uint32_t*)rtguest::G(0x2d9fd0)) = (long)(*(uint32_t*)rtguest::G(0x2da060)) | (long)(*(uint32_t*)rtguest::G(0x2da064)) << 0x20;
      (*(uint32_t*)rtguest::G(0x2d9ff0)) = (long)(*(uint32_t*)rtguest::G(0x2da070)) | (long)(*(uint32_t*)rtguest::G(0x2da07c)) << 0x38 |
                     (long)(*(uint32_t*)rtguest::G(0x2da074)) << 0x20 | (long)(*(uint32_t*)rtguest::G(0x2da07c)) << 0x18 |
                     (long)(*(uint32_t*)rtguest::G(0x2da078)) << 0x10 | (long)(*(uint32_t*)rtguest::G(0x2da078)) << 0x30;
      (*(uint32_t*)rtguest::G(0x2da010)) = (long)(*(uint32_t*)rtguest::G(0x2da080)) | (long)(*(uint32_t*)rtguest::G(0x2da084)) << 0x20;
    }
    else {
      iVar8 = iVar8 * (*(uint32_t*)rtguest::G(0x2da06a));
      iVar14 = (int)sVar7 * (int)(*(uint32_t*)rtguest::G(0x2da08a));
      iVar13 = iVar13 * (*(uint32_t*)rtguest::G(0x2da068));
      iVar16 = (int)sVar2 * (int)(*(uint32_t*)rtguest::G(0x2da088));
      if (iVar8 < 0) {
        iVar8 = iVar8 + 0xf;
      }
      if (iVar14 < 0) {
        iVar14 = iVar14 + 0xf;
      }
      if (iVar13 < 0) {
        iVar13 = iVar13 + 0xf;
      }
      if (iVar16 < 0) {
        iVar16 = iVar16 + 0xf;
      }
      (*(uint32_t*)rtguest::G(0x2d9fc0)) = (long)(iVar13 >> 4) & (uint32_t)(*(uint32_t*)rtguest::G(0x2da06c)) |
                     ((long)(iVar8 >> 4) & (uint32_t)(*(uint32_t*)rtguest::G(0x2da06e))) << 0x10;
      (*(uint32_t*)rtguest::G(0x2d9fb0)) = (long)(*(uint32_t*)rtguest::G(0x2da050)) | (long)(*(uint32_t*)rtguest::G(0x2da05c)) << 0x38 |
                     (long)(*(uint32_t*)rtguest::G(0x2da054)) << 0x20 | (long)(*(uint32_t*)rtguest::G(0x2da05c)) << 0x18 |
                     (long)(*(uint32_t*)rtguest::G(0x2da058)) << 0x10 | (long)(*(uint32_t*)rtguest::G(0x2da058)) << 0x30;
      (*(uint32_t*)rtguest::G(0x2d9fd0)) = (long)(*(uint32_t*)rtguest::G(0x2da060)) | (long)(*(uint32_t*)rtguest::G(0x2da064)) << 0x20;
      (*(uint32_t*)rtguest::G(0x2da000)) = (long)(iVar16 >> 4) & (uint32_t)(*(uint32_t*)rtguest::G(0x2da08c)) |
                     ((long)(iVar14 >> 4) & (uint32_t)(*(uint32_t*)rtguest::G(0x2da08e))) << 0x10;
      (*(uint32_t*)rtguest::G(0x2d9ff0)) = (long)(*(uint32_t*)rtguest::G(0x2da070)) | (long)(*(uint32_t*)rtguest::G(0x2da07c)) << 0x38 |
                     (long)(*(uint32_t*)rtguest::G(0x2da074)) << 0x20 | (long)(*(uint32_t*)rtguest::G(0x2da07c)) << 0x18 |
                     (long)(*(uint32_t*)rtguest::G(0x2da078)) << 0x10 | (long)(*(uint32_t*)rtguest::G(0x2da078)) << 0x30;
      (*(uint32_t*)rtguest::G(0x2da010)) = (long)(*(uint32_t*)rtguest::G(0x2da080)) | (long)(*(uint32_t*)rtguest::G(0x2da084)) << 0x20;
    }
    (*(uint32_t*)rtguest::G(0x2d9f60)) = (*(uint32_t*)rtguest::G(0x2d9f60)) & 0xffffffffffff8000 | 0xc;
    gifChainEndTag(0x2d9f50,0xd);
    sendTexturePackets(0x2d9f50);
    iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x28)));
    goto switchD_0026272c_default;
  case 2:
    goto LAB_002628dc;
  case 3:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 5))) = (uint32_t)(*(uint64_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(5) * sizeof(uint64_t))) & 0xfffffeff | 8;
    (*(uint32_t*)rtguest::G((uint32_t)puVar18)) = 4;
    goto switchD_0026272c_default;
  case 4:
    uiFade((int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 2))) << 0x18,iVar3,iVar3);
    if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 2))) < 0x80) {
      sVar7 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 2))) + 2;
LAB_00262fcc:
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 2))) = sVar7;
      iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x28)));
      goto switchD_0026272c_default;
    }
    uVar9 = 0xffffffff;
    break;
  case 5:
    uiFade((int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 2))) << 0x18,iVar3,iVar3);
    if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 2))) < 0x80) {
      sVar7 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 2))) + 0x10;
      goto LAB_00262fcc;
    }
    uVar9 = 7;
    break;
  case 6:
    uiFade((int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 2))) << 0x18,iVar3,iVar3);
    if (0 < (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 2)))) {
      sVar7 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 2))) + -0x10;
      goto LAB_00262fcc;
    }
    uVar9 = 2;
    break;
  case 7:
    uiFade(0xffffffff80000000,iVar3,iVar3);
    uVar9 = (*(uint32_t*)rtguest::G(0x177ad48));
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cbd)));
    if (bVar1 == 1) {
      SoundStopAllChannels();
      InitCameraTransforms(param_2,0x177ac50);
      ResetMode1E((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x28))));
      StoreIndexedPosition(0x44570000,0x42ca0000,(*(uint32_t*)rtguest::G(0x333194)),0,0);
      (*(uint32_t*)rtguest::G(0x177ad48)) = uVar9;
      SoundFadeOutChannels();
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb8))) = 0;
      uVar9 = 6;
    }
    else {
      if (bVar1 < 2) {
        if (bVar1 == 0) {
          (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 2))) = 0;
          iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x28)));
          (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x26))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x26))) + '\x01';
          (*(uint32_t*)rtguest::G((uint32_t)puVar18)) = 1;
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 5))) = (uint32_t)(*(uint64_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(5) * sizeof(uint64_t))) | 0x100;
        }
        else {
          iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x28)));
        }
        goto switchD_0026272c_default;
      }
      if (bVar1 != 2) {
        iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x28)));
        goto switchD_0026272c_default;
      }
      SoundStopAllChannels();
      InitCameraTransforms(param_2,0x177ac50);
      ResetMode1E((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x28))));
      (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) | 0x800;
      (*(uint32_t*)rtguest::G(0x177ad48)) = 0x23a0;
      StoreIndexedPosition((*(uint32_t*)rtguest::G(0x333198)),0x42ca0000,0x420c0000,0,0);
      SoundFadeOutChannels();
      uVar9 = 6;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb8))) = 0;
    }
    break;
  case 0xffffffff:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb4))) = 1;
  default:
    goto switchD_0026272c_default;
  }
  iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x28)));
  (*(uint32_t*)rtguest::G((uint32_t)puVar18)) = uVar9;
switchD_0026272c_default:
  raceHudDraw(iVar8);
  pvVar10 = (uint32_t)0x0;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb4))) != 0) {
    SoundStopAllChannels();
    uiFade(0xffffffff80000000);
    iVar17 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x18)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 5))) = (uint32_t)(*(uint64_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(5) * sizeof(uint64_t))) | 8;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 4))) = 0xffffffff;
    pvVar10 = (uint32_t)gs_tasks_kill(param_1);
  }
  return pvVar10;
}


RT_GUEST_FN(0x00262678, guest::texAnimCopyB)

}  // namespace guest
