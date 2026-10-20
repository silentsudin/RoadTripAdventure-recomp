// guest:0x002156d0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateAudioSettingsMenuTask(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(400);
    uint32_t uStack_190 = rtguest::allocFrame(8);
    uint32_t uStack_188 = rtguest::allocFrame(8);
    uint32_t uStack_180 = rtguest::allocFrame(8);
    uint32_t uStack_178 = rtguest::allocFrame(8);
uint32_t auStack_170 = rtguest::allocFrame(112);
    uint32_t iStack_100 = rtguest::allocFrame(4);
    uint32_t uStack_fc = rtguest::allocFrame(4);
    uint32_t uStack_f8 = rtguest::allocFrame(4);
    uint32_t uStack_f4 = rtguest::allocFrame(4);
    uint32_t uStack_f0 = rtguest::allocFrame(4);
    uint32_t uStack_ec = rtguest::allocFrame(4);
    uint32_t uStack_e8 = rtguest::allocFrame(4);
    uint32_t uStack_e4 = rtguest::allocFrame(4);
    uint32_t uStack_e0 = rtguest::allocFrame(8);
    uint32_t uStack_d8 = rtguest::allocFrame(4);
    uint32_t uStack_d4 = rtguest::allocFrame(4);
    uint32_t uStack_d0 = rtguest::allocFrame(4);
    uint32_t uStack_cc = rtguest::allocFrame(4);
    uint32_t uStack_c8 = rtguest::allocFrame(4);
    uint32_t uStack_c4 = rtguest::allocFrame(4);
    uint32_t uStack_c0 = rtguest::allocFrame(4);
    uint32_t uStack_bc = rtguest::allocFrame(4);
    uint32_t uStack_b8 = rtguest::allocFrame(4);
    uint32_t uStack_b4 = rtguest::allocFrame(4);
    uint32_t uStack_b0 = rtguest::allocFrame(4);
    uint32_t uStack_ac = rtguest::allocFrame(4);
    uint32_t uStack_a8 = rtguest::allocFrame(4);
    uint32_t uStack_a4 = rtguest::allocFrame(4);
    uint32_t uStack_a0 = rtguest::allocFrame(4);
    uint32_t uStack_9c = rtguest::allocFrame(4);
    uint32_t uStack_98 = rtguest::allocFrame(4);
    uint32_t uStack_94 = rtguest::allocFrame(4);
    uint32_t uStack_90 = rtguest::allocFrame(4);
    uint32_t uStack_8c = rtguest::allocFrame(4);
    uint32_t uStack_88 = rtguest::allocFrame(4);
    uint32_t uStack_84 = rtguest::allocFrame(4);
    uint32_t uStack_80 = rtguest::allocFrame(8);
    uint32_t fStack_78 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_70 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t iStack_68 = rtguest::allocFrame(4);
    uint32_t iStack_64 = rtguest::allocFrame(4);
    uint32_t uStack_60 = rtguest::allocFrame(4);
    uint32_t uStack_5c = rtguest::allocFrame(4);
uint32_t pcVar1;
  char cVar2;
  int iVar3;
  int iVar4;
uint32_t puVar5;
uint32_t puVar6;
uint32_t puVar7;
  long lVar8;
  uint uVar9;
uint32_t puVar10;
  undefined8 uVar11;
  undefined8 uVar12;
  undefined8 uVar13;
uint32_t puVar14;
  uint uVar15;
  int iVar16;
  int iVar17;
  int iVar18;
uint32_t pcVar19;
  int iVar20;
  int iVar21;
  int iVar22;
uint32_t puStack_6c;
  
  iVar18 = (int)param_3;
  puStack_6c = param_4;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 8))) == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_4)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(uint32_t))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_70)) = 3;
    InitMenuElementLayout(param_3,iVar18 + 0x820c,uStack_70,0,0);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x81d4))) = 0xfffff0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x81d8))) = 3;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x81cc))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x81d0))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x81c8))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x81c8))) | 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x81a4))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x81a4))) | 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x81b0))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x81a8))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x81ac))) = 0;
  }
  UpdateTimeOfDayCounter(param_2);
  lVar8 = HandleAudioSettingsMenuInput(param_1,param_2,param_3,puStack_6c);
  if (lVar8 == 399) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 4))) = 0;
  }
  iVar20 = iVar18 + 0x20;
  puVar14 = (uint32_t)(iVar18 + 0x81dc);
  uiArenaInit(iVar20,0x8000);
  uiArenaBeginBatch(iVar20,0,0);
  iVar16 = 1;
  DrawMenuGridBorder(param_3,iVar20,iVar18 + 0x81a4,1);
  DrawMenuListItems(param_3,iVar20,iVar18 + 0x81b4,1,param_2);
  DrawMenuGridBackground(param_3,iVar20,iVar18 + 0x81c8,1);
  do {
    if (((((*(uint32_t*)rtguest::G((uint32_t)puVar14)) & 1) != 0) && ((*(uint32_t*)rtguest::G((uint32_t)puVar14 + (uint32_t)(4) * sizeof(uint32_t))) != 0)) && (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 8))) & 0x10) != 0)) {
      uiArenaSetTexture(iVar20,0x29c030,0,0);
      uiArenaDataTag(iVar20,1);
      uiArenaSprites(iVar20,(*(uint32_t*)rtguest::G((uint32_t)puVar14 + (uint32_t)(5) * sizeof(uint32_t))) * 0x20 + 0x29c300,1,(*(uint32_t*)rtguest::G((uint32_t)puVar14 + (uint32_t)(1) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)puVar14 + (uint32_t)(2) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)puVar14 + (uint32_t)(3) * sizeof(uint32_t))));
    }
    iVar16 = iVar16 + -1;
    puVar14 = puVar14 + 0x18;  // T* element step
  } while (iVar16 != -1);
  DrawMenuCursorAndIcons(param_3,iVar20,iVar18 + 0x820c,0xb);
  uiArenaSetTexture(iVar20,0x29c030,3,0);
  iVar16 = 0x15;
  uiArenaDataTag(iVar20,0x18);
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(uStack_190)), 0, 0x20));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_180)) = 0x8000000060;
  uiArenaSprites(iVar20,uStack_190,1,0xa5,0xa0,0);
  (*(uint32_t*)rtguest::G((uint32_t)uStack_190 + 0)) = 0x60;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_190 + 4)) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_188)) = 0x60;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_180)) = 0x8000000008;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_178)) = 0;
  do {
    uiArenaSprites(iVar20,uStack_190,1,0xa5,0xa0,0);
    iVar16 = iVar16 + -1;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_190 + 0)) = (int)(*(uint64_t*)rtguest::G((uint32_t)uStack_190)) + (int)(*(uint64_t*)rtguest::G((uint32_t)uStack_180));
  } while (-1 < iVar16);
  (*(uint64_t*)rtguest::G((uint32_t)uStack_190)) = 0x110;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_188)) = 0x68;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_180)) = 0x8000000026;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_178)) = 0;
  uiArenaSprites(iVar20,uStack_190,1,0xa5,0xa0,0);
  uiArenaSetTexture(iVar20,0x29c030,0,0);
  uiArenaDataTag(iVar20,1);
  (*(uint32_t*)rtguest::G((uint32_t)uStack_ec)) = 0x16;
  iVar17 = 4;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_fc)) = 0xffffffe2;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_f4)) = 0x58;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_f0)) = 0x26;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_f8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
  (*(int32_t*)rtguest::G((uint32_t)iStack_100)) = (*(uint32_t*)rtguest::G((uint32_t)puStack_6c + (uint32_t)(2) * sizeof(uint32_t))) * 0x2e + 0x68;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_e4)) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_190)) = CONCAT44(0xffffffe2,(*(int32_t*)rtguest::G((uint32_t)iStack_100)));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_188)) = 0x5800000000;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_180)) = 0x1600000026;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_178)) = 0;
  uiArenaSprites(iVar20,uStack_190,1,0xa5,0xa0,0);
  uiArenaSetTexture(iVar20,0x29c030,0,0);
  uiArenaDataTag(iVar20,5);
  iVar16 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x14)));
  uVar15 = (((iVar16 / 90000) * 0x10 + (iVar16 / 9000) % 10) * 0x100 + (iVar16 % 9000) / 0x5dc) *
           0x10 + ((iVar16 % 9000) / 0x96) % 10 + 0xa00;
  iVar16 = 0xfa;
  do {
    uVar9 = uVar15 & 0xf;
    uVar15 = (int)uVar15 >> 4;
    iVar17 = iVar17 + -1;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_190)) = CONCAT44(0x3a,iVar16);
    (*(uint64_t*)rtguest::G((uint32_t)uStack_188)) = CONCAT44(0x60,uVar9 * 10 + 0x46);
    (*(uint64_t*)rtguest::G((uint32_t)uStack_180)) = 0xe0000000a;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_178)) = 0;
    uiArenaSprites(iVar20,uStack_190,1,0xa5,0xa0,0);
    iVar16 = iVar16 + -10;
  } while (-1 < iVar17);
  uiArenaSetTexture(iVar20,0x29c030,0,0);
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d8)) = 0x47;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_e0 + 4)) = 0x3e;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c0)) = 0xc0400000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_bc)) = 0xc1980000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = 0xc1980000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_a4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_a0)) = 0xc0400000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_9c)) = 0xc1500000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_94)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_8c)) = 0xc1500000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_84)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_e0 + 0)) = 0x36;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d4)) = 0x61;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d0)) = 8;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_cc)) = 0xc;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c4)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b0)) = 0x40400000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_a8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_98)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_90)) = 0x40400000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_88)) = 0;
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(uStack_80)), 0, 0x10));
  (*(float*)rtguest::G((uint32_t)fStack_78)) = (float)(int)(*(uint32_t*)rtguest::G((uint32_t)puStack_6c + (uint32_t)(4) * sizeof(uint32_t))) * (*(uint32_t*)rtguest::G(0x332b34)) * 0.0078125 + (*(uint32_t*)rtguest::G(0x332b38));
  puVar7 = uStack_e0;
  puVar5 = auStack_170;
  do {
    puVar10 = puVar5;
    puVar6 = puVar7;
    uVar11 = (*(uint64_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(1) * sizeof(uint64_t)));
    uVar12 = (*(uint64_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(2) * sizeof(uint64_t)));
    uVar13 = (*(uint64_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(3) * sizeof(uint64_t)));
    (*(uint64_t*)rtguest::G((uint32_t)puVar10)) = (*(uint64_t*)rtguest::G((uint32_t)puVar6));
    (*(uint64_t*)rtguest::G((uint32_t)puVar10 + (uint32_t)(1) * sizeof(uint64_t))) = uVar11;
    (*(uint64_t*)rtguest::G((uint32_t)puVar10 + (uint32_t)(2) * sizeof(uint64_t))) = uVar12;
    (*(uint64_t*)rtguest::G((uint32_t)puVar10 + (uint32_t)(3) * sizeof(uint64_t))) = uVar13;
    puVar7 = puVar6 + 4;
    puVar5 = puVar10 + 4;
  } while (puVar7 != uStack_80);
  uVar11 = (*(uint64_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(5) * sizeof(uint64_t)));
  (*(uint64_t*)rtguest::G((uint32_t)puVar10 + (uint32_t)(4) * sizeof(uint64_t))) = (*(uint64_t*)rtguest::G((uint32_t)puVar7));
  (*(uint64_t*)rtguest::G((uint32_t)puVar10 + (uint32_t)(5) * sizeof(uint64_t))) = uVar11;
  uiArenaRotQuads(iVar20,auStack_170,1,0xa5,0xa0,0);
  iVar16 = (*(uint32_t*)rtguest::G((uint32_t)puStack_6c + (uint32_t)(3) * sizeof(uint32_t)));
  iVar17 = 0;
  if (iVar16 != 0) {
    uiArenaSetTexture(iVar20,0x29c030,0,0);
    iVar16 = iVar16 + -1;
    uiArenaDataTag(iVar20,1);
    (*(uint64_t*)rtguest::G((uint32_t)uStack_190)) = 0x3800000070;
    (*(int32_t*)rtguest::G((uint32_t)iStack_68)) = 0;
    (*(int32_t*)rtguest::G((uint32_t)iStack_64)) = 0x6c;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_188)) = CONCAT44(0x6e,iVar16 * 0x48);
    (*(uint64_t*)rtguest::G((uint32_t)uStack_180)) = 0x1200000048;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_178)) = 0;
    uiArenaSprites(iVar20,uStack_190,1,0xa5,0xa0,0);
    pcVar19 = 0x0029c458 + (iVar16 * 4 + ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 8))) >> 5 & 3U)) * 0x10;
    uiArenaSetTexture(iVar20,0x29c030,0,0);
    do {
      pcVar1 = pcVar19 + 1;
      cVar2 = (*(int8_t*)rtguest::G((uint32_t)pcVar19));
      pcVar19 = pcVar19 + 2;
      if ((*(int8_t*)rtguest::G((uint32_t)pcVar1)) == '\0') {
        trap(7);
      }
      iVar16 = (int)cVar2 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 8))) % (int)(*(int8_t*)rtguest::G((uint32_t)pcVar1));
      if (iVar16 < 0) {
        iVar16 = 0;
      }
      else if (7 < iVar16) {
        iVar16 = 7;
      }
      if ((iVar16 != 0) && (uiArenaDataTag(iVar20,iVar16), iVar17 = (*(int32_t*)rtguest::G((uint32_t)iStack_64)), 0 < iVar16)) {
        (*(uint32_t*)rtguest::G((uint32_t)uStack_60)) = 0xd8;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) = 0;
        iVar21 = 0x6c;
        do {
          iVar16 = iVar16 + -1;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_190)) = CONCAT44(iVar21,iVar17);
          (*(uint64_t*)rtguest::G((uint32_t)uStack_188)) = CONCAT44(0x70,(*(uint32_t*)rtguest::G((uint32_t)uStack_60)));
          (*(uint64_t*)rtguest::G((uint32_t)uStack_180)) = 0x200000010;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_178)) = 0;
          uiArenaSprites(iVar20,uStack_190,1,0xa5,0xa0,0);
          iVar21 = iVar21 + -4;
        } while (iVar16 != 0);
      }
      (*(int32_t*)rtguest::G((uint32_t)iStack_68)) = (*(int32_t*)rtguest::G((uint32_t)iStack_68)) + 1;
      (*(int32_t*)rtguest::G((uint32_t)iStack_64)) = (*(int32_t*)rtguest::G((uint32_t)iStack_64)) + 0x14;
    } while ((*(int32_t*)rtguest::G((uint32_t)iStack_68)) < 8);
    iVar17 = (*(uint32_t*)rtguest::G((uint32_t)puStack_6c + (uint32_t)(3) * sizeof(uint32_t)));
  }
  iVar18 = 0;
  while( true ) {
    if (iVar18 == iVar17) {
      uiArenaSetTexture(iVar20,0x29c030,3,1);
      uVar15 = 0x90;
    }
    else {
      uiArenaSetTexture(iVar20,0x29c030,3,2);
      uVar15 = 0xc0;
    }
    (*(uint64_t*)rtguest::G((uint32_t)uStack_190)) = CONCAT44(8,iVar18 * 0x2e + 100);
    (*(uint64_t*)rtguest::G((uint32_t)uStack_180)) = 0x240000002e;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_188)) = (uint32_t)uVar15;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_178)) = 0;
    uiArenaDataTag(iVar20,1);
    iVar18 = iVar18 + 1;
    uiArenaSprites(iVar20,uStack_190,1,0xa5,0xa0,0);
    if (2 < iVar18) break;
    iVar17 = (*(uint32_t*)rtguest::G((uint32_t)puStack_6c + (uint32_t)(3) * sizeof(uint32_t)));
  }
  iVar18 = 0;
  iVar22 = 0xb0;
  iVar21 = 0x72;
  iVar17 = 0x80;
  iVar16 = 0x72;
  do {
    if (iVar18 == (*(uint32_t*)rtguest::G((uint32_t)puStack_6c + (uint32_t)(3) * sizeof(uint32_t)))) {
      uiArenaSetTexture(iVar20,0x29c030,3,1);
      iVar3 = iVar16;
      iVar4 = iVar17;
      if (iVar18 == 0) {
        (*(uint64_t*)rtguest::G((uint32_t)uStack_190)) = 0x140000006b;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_188)) = 0x6000000090;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_180)) = 0xc00000020;
      }
      else {
LAB_00216164:
        (*(uint64_t*)rtguest::G((uint32_t)uStack_188)) = CONCAT44(0x24,iVar4);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_190)) = CONCAT44(0x14,iVar3);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_180)) = 0xc00000010;
      }
    }
    else {
      uiArenaSetTexture(iVar20,0x29c030,3,2);
      iVar3 = iVar21;
      iVar4 = iVar22;
      if (iVar18 != 0) goto LAB_00216164;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_190)) = 0x140000006b;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_188)) = 0x7000000090;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_180)) = 0xc00000020;
    }
    (*(uint64_t*)rtguest::G((uint32_t)uStack_178)) = 0;
    uiArenaDataTag(iVar20,1);
    iVar18 = iVar18 + 1;
    uiArenaSprites(iVar20,uStack_190,1,0xa5,0xa0,0);
    iVar22 = iVar22 + 0x10;
    iVar21 = iVar21 + 0x2e;
    iVar17 = iVar17 + 0x10;
    iVar16 = iVar16 + 0x2e;
    if (2 < iVar18) {
      uiArenaSubmit(iVar20);
      return 0;
    }
  } while( true );
}


RT_GUEST_FN(0x002156d0, guest::UpdateAudioSettingsMenuTask)

}  // namespace guest
