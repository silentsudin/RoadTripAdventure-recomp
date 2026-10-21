// guest:0x00237c48 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateRaceRecords(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x28, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x38, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0x40, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x48) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4c) = 0x00000000u;
    rtguest::setS32(5, (uint32_t)param_1);  // walk: = param_1
  }
  byte bVar1;
  char cVar2;
  short sVar3;
  uint uVar4;
  undefined4 uVar5;
  long lVar6;
  int iVar7;
  uint uVar8;
  int iVar9;
  undefined1 uVar10;
  int iVar11;
  uint uVar12;
uint32_t pcVar13;
uint32_t puVar14;
  int iVar15;
uint32_t pbVar16;
uint32_t puVar17;
uint32_t pcVar18;
  int iVar19;
uint32_t puVar20;
  uint uVar21;
  int iVar22;
  int iVar23;
uint32_t puVar24;
uint32_t pbVar25;
  
  bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 10)));
  uVar21 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20)));
  if ((bVar1 == 0) && (uVar21 < 0x1c)) {
    if (0x19 < uVar21) {
      uVar21 = uVar21 - 2;
    }
    iVar9 = uVar21 * 4;
    puVar17 = 0x01787074;
    pbVar16 = 0x0178700a;
    pbVar25 = 0x01780750 + uVar21;
    puVar14 = (uint32_t)(0x017808d8 + iVar9);
    puVar24 = (uint32_t)(0x017807a0 + iVar9);
    iVar19 = 0;
    puVar20 = 0x01786ff0;
    iVar23 = 0;
    pcVar13 = (uint32_t)((int)0x0177fdd0 + 3);
    pcVar18 = (uint32_t)((int)0x0177fdd0 + 2);
    iVar22 = 0;
    uVar12 = uVar21;
    do {
      if (iVar19 == 0) {
        iVar11 = 0;
        iVar15 = 0;
      }
      else {
        iVar7 = (uVar21 * 2 + iVar19) * 2;
        iVar15 = iVar22;
        if ((((*(uint32_t*)rtguest::G((uint32_t)0x1782b1e + (uint32_t)(iVar7) * 4)) != (*(int8_t*)rtguest::G((uint32_t)pcVar18))) ||
            (iVar11 = iVar19 << 2, (*(uint32_t*)rtguest::G((uint32_t)0x1782b1f + (uint32_t)(iVar7) * 4)) != (*(int8_t*)rtguest::G((uint32_t)pcVar13)))) &&
           ((iVar11 = iVar23, (*(int8_t*)rtguest::G((uint32_t)pbVar16)) < (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x1780750 + (uint32_t)(uVar12) * 4)) ||
            (((*(uint32_t*)rtguest::G((uint32_t)puVar20)) < (*(uint32_t*)rtguest::G((uint32_t)puVar14)) || ((*(uint32_t*)rtguest::G((uint32_t)puVar17)) < (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017807a0 + iVar9))))))))) {
          uVar8 = (*(uint32_t*)rtguest::G((uint32_t)puVar17));
          iVar7 = (uVar21 * 2 + iVar19) * 2;
          uVar4 = (*(uint32_t*)rtguest::G((uint32_t)puVar20));
          bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar16));
          (*(uint32_t*)rtguest::G((uint32_t)0x1782b1e + (uint32_t)(iVar7) * 4)) = (*(int8_t*)rtguest::G((uint32_t)pcVar18));
          (*(uint32_t*)rtguest::G((uint32_t)0x1782b1f + (uint32_t)(iVar7) * 4)) = (*(int8_t*)rtguest::G((uint32_t)pcVar13));
          (*(uint32_t*)rtguest::G((uint32_t)puVar24)) = uVar8;
          (*(uint32_t*)rtguest::G((uint32_t)puVar14)) = uVar4;
          (*(int8_t*)rtguest::G((uint32_t)pbVar25)) = bVar1;
        }
      }
      if ((*(int8_t*)rtguest::G((uint32_t)pbVar16)) < (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x1780750 + (uint32_t)(uVar12) * 4))) {
        (*(uint32_t*)rtguest::G((uint32_t)0x1780750 + (uint32_t)(uVar12) * 4)) = (*(int8_t*)rtguest::G((uint32_t)pbVar16));
      }
      if ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x01786ff0 + iVar11))) < (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017808d8 + uVar12 * 4)))) {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017808d8 + uVar12 * 4))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x01786ff0 + iVar11)));
      }
      if ((*(uint32_t*)rtguest::G((uint32_t)puVar17)) < (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017807a0 + uVar12 * 4)))) {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017807a0 + uVar12 * 4))) = (*(uint32_t*)rtguest::G((uint32_t)puVar17));
      }
      iVar19 = iVar19 + 1;
      if ((*(int8_t*)rtguest::G((uint32_t)0x177fdd4 + (uint32_t)(iVar15) * 1)) == '\0') break;
      puVar17 = puVar17 + 0x10;  // T* element step
      pbVar16 = pbVar16 + 4;
      uVar12 = uVar12 + 0x1a;
      pbVar25 = pbVar25 + 0x1a;
      puVar14 = puVar14 + 0x68;  // T* element step
      puVar20 = puVar20 + 0x4;  // T* element step
      iVar23 = iVar23 + 4;
      puVar24 = puVar24 + 0x68;  // T* element step
      iVar9 = iVar9 + 0x68;
      pcVar13 = pcVar13 + 2;
      pcVar18 = pcVar18 + 2;
      iVar22 = iVar22 + 2;
    } while (iVar19 < 3);
    iVar9 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10)));
    iVar19 = 1;
    if (1 < (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 1)))) {
      iVar22 = 4;
      do {
        iVar19 = iVar19 + 1;
        if ((*(uint8_t*)rtguest::G(0x178700a)) < (*(uint8_t*)rtguest::G((uint32_t)0x178700a + (uint32_t)(iVar22) * 1))) {
          RT_CALLRA(0x237efc, guest::SetFlagFcb8((*(uint32_t*)rtguest::G((uint32_t)0x1787008 + (uint32_t)(iVar22) * 4)),(*(uint32_t*)rtguest::G((uint32_t)0x1787009 + (uint32_t)(iVar22) * 4))));
          iVar9 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10)));
        }
        iVar22 = iVar19 * 4;
      } while (iVar19 < (int)(uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 1))));
    }
  }
  else if (bVar1 == 1) {
    iVar9 = uVar21 * 0xc;
    if ((short)(uint32_t)(*(uint8_t*)rtguest::G(0x178700a)) < (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(0x017802e2 + iVar9)))) {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x017802e2 + iVar9))) = (uint32_t)(*(uint8_t*)rtguest::G(0x178700a));
    }
    if ((int)(*(uint32_t*)rtguest::G(0x1786ff0)) < (int)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017802dc + iVar9)))) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017802dc + iVar9))) = (*(uint32_t*)rtguest::G(0x1786ff0));
    }
    if ((int)(*(uint32_t*)rtguest::G(0x1787074)) < (int)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017802d8 + iVar9)))) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017802d8 + iVar9))) = (*(uint32_t*)rtguest::G(0x1787074));
    }
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(0x017802e0 + iVar9))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(0x017802e0 + iVar9))) + 1;
  }
  else if ((bVar1 & 2) != 0) {
    iVar9 = uVar21 * 0x20;
    if ((int)(*(uint32_t*)rtguest::G(0x1786ff0)) < (int)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801b4 + iVar9)))) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801b4 + iVar9))) = (*(uint32_t*)rtguest::G(0x1786ff0));
    }
    uVar12 = (*(uint32_t*)rtguest::G(0x1787074));
    if ((int)(*(uint32_t*)rtguest::G(0x1787074)) < (int)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801b0 + iVar9)))) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801b0 + iVar9))) = (*(uint32_t*)rtguest::G(0x1787074));
    }
    if ((int)(*(uint32_t*)rtguest::G(0x1786ffc)) < (int)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835fc + iVar9)))) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835fc + iVar9))) = (*(uint32_t*)rtguest::G(0x1786ffc));
    }
    if ((int)(*(uint32_t*)rtguest::G(0x17870a4)) < (int)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835f8 + iVar9)))) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835f8 + iVar9))) = (*(uint32_t*)rtguest::G(0x17870a4));
    }
    cVar2 = (*(uint8_t*)rtguest::G((uint32_t)0x177fdfe + (uint32_t)(uVar21 + (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) * 0x30) * 1));
    if (cVar2 == -1) {
      sVar3 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1e)));
    }
    else {
      (*(uint8_t*)rtguest::G((uint32_t)0x177fdfe + (uint32_t)(uVar21 + (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) * 0x30) * 1)) = cVar2 + '\x01';
      sVar3 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1e)));
    }
    cVar2 = (*(uint32_t*)rtguest::G((uint32_t)0x1783246 + (uint32_t)(uVar21 + sVar3 * 0x30) * 4));
    if (cVar2 != -1) {
      (*(uint32_t*)rtguest::G((uint32_t)0x1783246 + (uint32_t)(uVar21 + sVar3 * 0x30) * 4)) = cVar2 + '\x01';
    }
    uVar8 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x01780198 + iVar9)));
    if (((uVar8 & 0xffffff) == 0) || ((int)uVar12 < (int)(uVar8 << 8) >> 8)) {
      uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0178019c + iVar9)));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0178019c + iVar9))) = uVar8;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x01780198 + iVar9))) = uVar8 & 0xff000000 | uVar12 & 0xffffff;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801a0 + iVar9))) = uVar5;
      (*(uint32_t*)rtguest::G((uint32_t)0x178019b + (uint32_t)(iVar9) * 4)) = 0xff;
    }
    else {
      uVar8 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0178019c + iVar9)));
      if (((uVar8 & 0xffffff) == 0) || ((int)uVar12 < (int)(uVar8 << 8) >> 8)) {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801a0 + iVar9))) = uVar8;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0178019c + iVar9))) = uVar8 & 0xff000000 | uVar12 & 0xffffff;
        (*(uint32_t*)rtguest::G((uint32_t)0x178019f + (uint32_t)(iVar9) * 4)) = 0xff;
      }
      else {
        uVar8 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801a0 + iVar9)));
        if (((uVar8 & 0xffffff) == 0) || ((int)uVar12 < (int)(uVar8 << 8) >> 8)) {
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801a0 + iVar9))) = uVar8 & 0xff000000 | uVar12 & 0xffffff;
          (*(uint32_t*)rtguest::G((uint32_t)0x17801a3 + (uint32_t)(iVar9) * 4)) = 0xff;
        }
      }
    }
    uVar12 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x01780198 + iVar9)));
    uVar10 = (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
    if (((uVar12 & 0xffffff) == 0) || ((int)(*(uint32_t*)rtguest::G(0x17870a4)) < (int)(uVar12 << 8) >> 8)) {
      uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0178019c + iVar9)));
      uVar8 = (*(uint32_t*)rtguest::G(0x17870a4)) & 0xffffff;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0178019c + iVar9))) = uVar12;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x01780198 + iVar9))) = uVar12 & 0xff000000 | uVar8;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801a0 + iVar9))) = uVar5;
      (*(uint32_t*)rtguest::G((uint32_t)0x178019b + (uint32_t)(iVar9) * 4)) = uVar10;
    }
    else {
      uVar12 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0178019c + iVar9)));
      if (((uVar12 & 0xffffff) == 0) || ((int)(*(uint32_t*)rtguest::G(0x17870a4)) < (int)(uVar12 << 8) >> 8)) {
        uVar8 = (*(uint32_t*)rtguest::G(0x17870a4)) & 0xffffff;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801a0 + iVar9))) = uVar12;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0178019c + iVar9))) = uVar12 & 0xff000000 | uVar8;
        (*(uint32_t*)rtguest::G((uint32_t)0x178019f + (uint32_t)(iVar9) * 4)) = uVar10;
      }
      else {
        uVar12 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801a0 + iVar9)));
        if (((uVar12 & 0xffffff) == 0) || ((int)(*(uint32_t*)rtguest::G(0x17870a4)) < (int)(uVar12 << 8) >> 8)) {
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801a0 + iVar9))) = uVar12 & 0xff000000 | (*(uint32_t*)rtguest::G(0x17870a4)) & 0xffffff;
          (*(uint32_t*)rtguest::G((uint32_t)0x17801a3 + (uint32_t)(iVar9) * 4)) = uVar10;
        }
      }
    }
    uVar12 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835e0 + iVar9)));
    uVar10 = (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1e)));
    if (((uVar12 & 0xffffff) == 0) || ((int)(*(uint32_t*)rtguest::G(0x1787074)) < (int)(uVar12 << 8) >> 8)) {
      uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835e4 + iVar9)));
      uVar8 = (*(uint32_t*)rtguest::G(0x1787074)) & 0xffffff;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835e4 + iVar9))) = uVar12;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835e0 + iVar9))) = uVar12 & 0xff000000 | uVar8;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835e8 + iVar9))) = uVar5;
      (*(uint32_t*)rtguest::G((uint32_t)0x17835e3 + (uint32_t)(iVar9) * 4)) = uVar10;
    }
    else {
      uVar12 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835e4 + iVar9)));
      if (((uVar12 & 0xffffff) == 0) || ((int)(*(uint32_t*)rtguest::G(0x1787074)) < (int)(uVar12 << 8) >> 8)) {
        uVar8 = (*(uint32_t*)rtguest::G(0x1787074)) & 0xffffff;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835e8 + iVar9))) = uVar12;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835e4 + iVar9))) = uVar12 & 0xff000000 | uVar8;
        (*(uint32_t*)rtguest::G((uint32_t)0x17835e7 + (uint32_t)(iVar9) * 4)) = uVar10;
      }
      else {
        uVar12 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835e8 + iVar9)));
        if (((uVar12 & 0xffffff) == 0) || ((int)(*(uint32_t*)rtguest::G(0x1787074)) < (int)(uVar12 << 8) >> 8)) {
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835e8 + iVar9))) = uVar12 & 0xff000000 | (*(uint32_t*)rtguest::G(0x1787074)) & 0xffffff;
          (*(uint32_t*)rtguest::G((uint32_t)0x17835eb + (uint32_t)(iVar9) * 4)) = uVar10;
        }
      }
    }
    uVar12 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835e0 + iVar9)));
    if (((uVar12 & 0xffffff) == 0) || ((int)(*(uint32_t*)rtguest::G(0x17870a4)) < (int)(uVar12 << 8) >> 8)) {
      uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835e4 + iVar9)));
      uVar8 = (*(uint32_t*)rtguest::G(0x17870a4)) & 0xffffff;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835e4 + iVar9))) = uVar12;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835e0 + iVar9))) = uVar12 & 0xff000000 | uVar8;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835e8 + iVar9))) = uVar5;
      (*(uint32_t*)rtguest::G((uint32_t)0x17835e3 + (uint32_t)(iVar9) * 4)) = 0xff;
    }
    else {
      uVar12 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835e4 + iVar9)));
      if (((uVar12 & 0xffffff) == 0) || ((int)(*(uint32_t*)rtguest::G(0x17870a4)) < (int)(uVar12 << 8) >> 8)) {
        uVar8 = (*(uint32_t*)rtguest::G(0x17870a4)) & 0xffffff;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835e8 + iVar9))) = uVar12;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835e4 + iVar9))) = uVar12 & 0xff000000 | uVar8;
        (*(uint32_t*)rtguest::G((uint32_t)0x17835e7 + (uint32_t)(iVar9) * 4)) = 0xff;
      }
      else {
        uVar12 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835e8 + iVar9)));
        if (((uVar12 & 0xffffff) == 0) || ((int)(*(uint32_t*)rtguest::G(0x17870a4)) < (int)(uVar12 << 8) >> 8)) {
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835e8 + iVar9))) = uVar12 & 0xff000000 | (*(uint32_t*)rtguest::G(0x17870a4)) & 0xffffff;
          (*(uint32_t*)rtguest::G((uint32_t)0x17835eb + (uint32_t)(iVar9) * 4)) = 0xff;
        }
      }
    }
    uVar12 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801a4 + iVar9)));
    if (((uVar12 & 0xffffff) == 0) || ((int)(*(uint32_t*)rtguest::G(0x1786ff0)) < (int)(uVar12 << 8) >> 8)) {
      uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801a8 + iVar9)));
      uVar8 = (*(uint32_t*)rtguest::G(0x1786ff0)) & 0xffffff;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801a8 + iVar9))) = uVar12;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801a4 + iVar9))) = uVar12 & 0xff000000 | uVar8;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801ac + iVar9))) = uVar5;
      (*(uint32_t*)rtguest::G((uint32_t)0x17801a7 + (uint32_t)(iVar9) * 4)) = 0xff;
    }
    else {
      uVar12 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801a8 + iVar9)));
      if (((uVar12 & 0xffffff) == 0) || ((int)(*(uint32_t*)rtguest::G(0x1786ff0)) < (int)(uVar12 << 8) >> 8)) {
        uVar8 = (*(uint32_t*)rtguest::G(0x1786ff0)) & 0xffffff;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801ac + iVar9))) = uVar12;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801a8 + iVar9))) = uVar12 & 0xff000000 | uVar8;
        (*(uint32_t*)rtguest::G((uint32_t)0x17801ab + (uint32_t)(iVar9) * 4)) = 0xff;
      }
      else {
        uVar12 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801ac + iVar9)));
        if (((uVar12 & 0xffffff) == 0) || ((int)(*(uint32_t*)rtguest::G(0x1786ff0)) < (int)(uVar12 << 8) >> 8)) {
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801ac + iVar9))) = uVar12 & 0xff000000 | (*(uint32_t*)rtguest::G(0x1786ff0)) & 0xffffff;
          (*(uint32_t*)rtguest::G((uint32_t)0x17801af + (uint32_t)(iVar9) * 4)) = 0xff;
        }
      }
    }
    uVar12 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801a4 + iVar9)));
    uVar10 = (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
    if (((uVar12 & 0xffffff) == 0) || ((int)(*(uint32_t*)rtguest::G(0x1786ffc)) < (int)(uVar12 << 8) >> 8)) {
      uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801a8 + iVar9)));
      uVar8 = (*(uint32_t*)rtguest::G(0x1786ffc)) & 0xffffff;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801a8 + iVar9))) = uVar12;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801a4 + iVar9))) = uVar12 & 0xff000000 | uVar8;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801ac + iVar9))) = uVar5;
      (*(uint32_t*)rtguest::G((uint32_t)0x17801a7 + (uint32_t)(iVar9) * 4)) = uVar10;
    }
    else {
      uVar12 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801a8 + iVar9)));
      if (((uVar12 & 0xffffff) == 0) || ((int)(*(uint32_t*)rtguest::G(0x1786ffc)) < (int)(uVar12 << 8) >> 8)) {
        uVar8 = (*(uint32_t*)rtguest::G(0x1786ffc)) & 0xffffff;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801ac + iVar9))) = uVar12;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801a8 + iVar9))) = uVar12 & 0xff000000 | uVar8;
        (*(uint32_t*)rtguest::G((uint32_t)0x17801ab + (uint32_t)(iVar9) * 4)) = uVar10;
      }
      else {
        uVar12 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801ac + iVar9)));
        if (((uVar12 & 0xffffff) == 0) || ((int)(*(uint32_t*)rtguest::G(0x1786ffc)) < (int)(uVar12 << 8) >> 8)) {
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801ac + iVar9))) = uVar12 & 0xff000000 | (*(uint32_t*)rtguest::G(0x1786ffc)) & 0xffffff;
          (*(uint32_t*)rtguest::G((uint32_t)0x17801af + (uint32_t)(iVar9) * 4)) = uVar10;
        }
      }
    }
    uVar12 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835ec + iVar9)));
    uVar10 = (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1e)));
    if (((uVar12 & 0xffffff) == 0) || ((int)(*(uint32_t*)rtguest::G(0x1786ff0)) < (int)(uVar12 << 8) >> 8)) {
      uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835f0 + iVar9)));
      uVar8 = (*(uint32_t*)rtguest::G(0x1786ff0)) & 0xffffff;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835f0 + iVar9))) = uVar12;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835ec + iVar9))) = uVar12 & 0xff000000 | uVar8;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835f4 + iVar9))) = uVar5;
      (*(uint32_t*)rtguest::G((uint32_t)0x17835ef + (uint32_t)(iVar9) * 4)) = uVar10;
    }
    else {
      uVar12 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835f0 + iVar9)));
      if (((uVar12 & 0xffffff) == 0) || ((int)(*(uint32_t*)rtguest::G(0x1786ff0)) < (int)(uVar12 << 8) >> 8)) {
        uVar8 = (*(uint32_t*)rtguest::G(0x1786ff0)) & 0xffffff;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835f4 + iVar9))) = uVar12;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835f0 + iVar9))) = uVar12 & 0xff000000 | uVar8;
        (*(uint32_t*)rtguest::G((uint32_t)0x17835f3 + (uint32_t)(iVar9) * 4)) = uVar10;
      }
      else {
        uVar12 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835f4 + iVar9)));
        if (((uVar12 & 0xffffff) == 0) || ((int)(*(uint32_t*)rtguest::G(0x1786ff0)) < (int)(uVar12 << 8) >> 8)) {
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835f4 + iVar9))) = uVar12 & 0xff000000 | (*(uint32_t*)rtguest::G(0x1786ff0)) & 0xffffff;
          (*(uint32_t*)rtguest::G((uint32_t)0x17835f7 + (uint32_t)(iVar9) * 4)) = uVar10;
        }
      }
    }
    uVar12 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835ec + iVar9)));
    if (((uVar12 & 0xffffff) == 0) || ((int)(*(uint32_t*)rtguest::G(0x1786ffc)) < (int)(uVar12 << 8) >> 8)) {
      uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835f0 + iVar9)));
      uVar8 = (*(uint32_t*)rtguest::G(0x1786ffc)) & 0xffffff;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835f0 + iVar9))) = uVar12;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835ec + iVar9))) = uVar12 & 0xff000000 | uVar8;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835f4 + iVar9))) = uVar5;
      (*(uint32_t*)rtguest::G((uint32_t)0x17835ef + (uint32_t)(iVar9) * 4)) = 0xff;
    }
    else {
      uVar12 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835f0 + iVar9)));
      if (((uVar12 & 0xffffff) == 0) || ((int)(*(uint32_t*)rtguest::G(0x1786ffc)) < (int)(uVar12 << 8) >> 8)) {
        uVar8 = (*(uint32_t*)rtguest::G(0x1786ffc)) & 0xffffff;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835f4 + iVar9))) = uVar12;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835f0 + iVar9))) = uVar12 & 0xff000000 | uVar8;
        (*(uint32_t*)rtguest::G((uint32_t)0x17835f3 + (uint32_t)(iVar9) * 4)) = 0xff;
      }
      else {
        uVar12 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835f4 + iVar9)));
        if (((uVar12 & 0xffffff) == 0) || ((int)(*(uint32_t*)rtguest::G(0x1786ffc)) < (int)(uVar12 << 8) >> 8)) {
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835f4 + iVar9))) = uVar12 & 0xff000000 | (*(uint32_t*)rtguest::G(0x1786ffc)) & 0xffffff;
          (*(uint32_t*)rtguest::G((uint32_t)0x17835f7 + (uint32_t)(iVar9) * 4)) = 0xff;
        }
      }
    }
    if (uVar21 == 6) {
      if ((*(uint8_t*)rtguest::G(0x178700a)) < (*(uint32_t*)rtguest::G(0x178700e))) {
        rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2ec560u)));
        pcVar13 = 0x0177fdfa + (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) * 0x30;
      }
      else {
        rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2ec570u)));
        pcVar13 = 0x01783242 + (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1e))) * 0x30;
      }
      if ((*(int8_t*)rtguest::G((uint32_t)pcVar13)) != -1) {
        (*(int8_t*)rtguest::G((uint32_t)pcVar13)) = (*(int8_t*)rtguest::G((uint32_t)pcVar13)) + '\x01';
      }
    }
    else {
      lVar6 = RT_CALLRA(0x238830, guest::GetOrSetPresidentRaceResult(0xffffffffffffffff));
      if (lVar6 == 1) {
        rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2ec570u)));
        pcVar13 = 0x0178323c + uVar21 + (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1e))) * 0x30;
      }
      else {
        if (lVar6 != 2) {
          return 0;
        }
        rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2ec560u)));
        pcVar13 = 0x0177fdf4 + uVar21 + (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) * 0x30;
      }
      if ((*(int8_t*)rtguest::G((uint32_t)pcVar13)) != -1) {
        (*(int8_t*)rtguest::G((uint32_t)pcVar13)) = (*(int8_t*)rtguest::G((uint32_t)pcVar13)) + '\x01';
      }
    }
  }
  return 0;
}


RT_GUEST_FN(0x00237c48, guest::UpdateRaceRecords)

}  // namespace guest
