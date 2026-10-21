// guest:0x00218458 transliterated from Ghidra pseudocode + ELF disassembly.
// Ghidra `(&DAT_x)[slot*0x9c]`/`[slot*0x138]` decays all equal slot*0x270 byte
// offsets (car block stride 624) => use iVar9 throughout. __s is a GUEST address
// (gs_controls_car_object 0x177ac50), never a host pointer: RAM pointer fields
// hold guest addresses (cf. 002080d8 storing 0x177f760).
// shortcut: *(code**) task+0xc / +0x2c stores keep guest addresses (0x21ba70,
// 0x21cbb8); resume-point dispatch is W12 dispatcher work.
#include "../guest_vu0.h"
#include "guest_decls.inc"

namespace guest {

void SpawnCarEntityTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x20, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x28) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x2c) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(2, (uint32_t)param_1);  // walk: = param_1
  }
  using namespace rtguest::vu0;
  byte bVar1;
  byte bVar2;
  short sVar3;
  ushort uVar4;
  ushort uVar5;
  uint32_t puVar6;
  undefined1 uVar8;
  int iVar9;
  char cVar10;
  uint uVar11;
  uint32_t puVar12;
  int iVar13;
  uint uVar14;
  uint32_t puVar15;
  int iVar16;
  uint32_t __s;
  int iVar17;
  int iVar18;
  undefined4 uVar19;
  float fVar20;
  float fVar21;
  uint32_t auStack_30 = rtguest::allocFrame(16);  // auStack_30 + auStack_2c contiguous
  uint32_t auStack_2c = auStack_30 + 4;

  iVar18 = (int)param_1;
  uVar11 = (*(uint32_t*)rtguest::G((uint32_t)(iVar18 + 0x1c))) & 0x1f;
  iVar9 = uVar11 * 0x270;
  __s = 0x00177ac50 + iVar9;  // gs_controls_car_object + slot*0x4e (undefined8 decay)
  ::memset((char *)rtguest::G((uint32_t)__s), 0, 0x270);
  iVar13 = (*(int32_t*)rtguest::G((uint32_t)(iVar18 + 0x1c)));
  uVar14 = (uint)iVar13 >> 5 & 0x1f;
  (*(uint16_t*)rtguest::G((uint32_t)(0x00177ade8 + iVar9))) = (short)((uint)iVar13 >> 0x10);
  (*(uint32_t*)rtguest::G((uint32_t)(0x00177ade4 + iVar9))) = uVar14;
  iVar17 = (int)param_2;
  if (((uint)iVar13 >> 0x10 & 0x33U) == 0) {
    if (((*(int8_t*)rtguest::G((uint32_t)(iVar17 + 0x22))) == -1) ||
        ((*(int8_t*)rtguest::G((uint32_t)(iVar17 + 0x22))) == '\"')) {
      (*(uint32_t*)rtguest::G((uint32_t)(0x00177add0 + iVar9))) = 0x0029cba0;
      puVar12 = (*(uint32_t*)rtguest::G((uint32_t)(0x00177ea50 + uVar14 * 4)));
    }
    else {
      puVar12 = 0x00177add8 + iVar9;
      puVar6 = (*(uint32_t*)rtguest::G((uint32_t)(0x00177ea50 + uVar14 * 4)));
      iVar13 = (*(int32_t*)rtguest::G((uint32_t)((*(uint32_t*)rtguest::G((uint32_t)(iVar17 + 0x10))) + 4)));
      uVar19 = (*(uint32_t*)rtguest::G((uint32_t)(puVar6 + 8)));
      (*(uint64_t*)rtguest::G((uint32_t)puVar12)) = (*(uint64_t*)rtguest::G((uint32_t)puVar6));
      (*(uint32_t*)rtguest::G((uint32_t)(0x00177ade0 + iVar9))) = uVar19;
      (*(uint32_t*)rtguest::G((uint32_t)(0x00177ea50 + uVar14 * 4))) = puVar12;
      uVar4 = (*(uint16_t*)rtguest::G((uint32_t)(iVar13 + 0x12)));
      (*(uint32_t*)rtguest::G((uint32_t)(0x00177add0 + iVar9))) = iVar13 + 4;
      (*(uint16_t*)rtguest::G((uint32_t)(0x00177ade0 + iVar9))) = uVar4;
    }
LAB_002186c8:
    (*(uint32_t*)rtguest::G((uint32_t)(0x00177add4 + iVar9))) = puVar12;
  }
  else {
    bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(iVar17 + 10)));
    if ((bVar1 == 0) && ((*(int8_t*)rtguest::G((uint32_t)(iVar17 + 0x22))) == '\x1c')) {
      iVar13 = (*(int32_t*)rtguest::G((uint32_t)((*(uint32_t*)rtguest::G((uint32_t)(iVar17 + 0x10))) + 4)));
      (*(uint32_t*)rtguest::G((uint32_t)(0x00177add4 + iVar9))) =
          (*(uint32_t*)rtguest::G(0x00177ea5c));
      (*(uint32_t*)rtguest::G((uint32_t)(0x00177ade4 + iVar9))) = 3;
      (*(uint32_t*)rtguest::G((uint32_t)(0x00177add0 + iVar9))) = iVar13 + 4;
    }
    else {
      if (((bVar1 & 2) == 0) || (bVar1 == 6)) {
        uVar5 = (*(uint16_t*)rtguest::G((uint32_t)(0x00177ade8 + iVar9)));
        if (bVar1 != 1) {
          iVar13 = 0x3448;
          if ((uVar5 & 0x40) == 0) {
            iVar13 = 0;
          }
          iVar13 = iVar13 + (uint)((uVar5 & 0x30) >> 4) * 0x1c;
          puVar12 = 0x00177f760 + iVar13;  // gs_progress_ram_address
          (*(uint32_t*)rtguest::G((uint32_t)(0x00177add0 + iVar9))) = 0x00177f76c + iVar13;
          goto LAB_002186c8;
        }
        bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(iVar17 + 0x20)));
        iVar13 = 0x3448;
        if ((uVar5 & 0x40) == 0) {
          iVar13 = 0;
        }
        iVar16 = 0x002be318;
      }
      else {
        uVar5 = (*(uint16_t*)rtguest::G((uint32_t)(0x00177ade8 + iVar9)));
        bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(iVar17 + 0x20)));
        iVar13 = 0x3448;
        if ((uVar5 & 0x40) == 0) {
          iVar13 = 0;
        }
        iVar16 = 0x002be278;
      }
      puVar12 = 0x00177add8 + iVar9;
      iVar13 = (uint)((uVar5 & 0x30) >> 4) * 0x1c + iVar13;
      uVar19 = (*(uint32_t*)rtguest::G((uint32_t)(0x00177f768 + iVar13)));
      (*(uint64_t*)rtguest::G((uint32_t)puVar12)) = (*(uint64_t*)rtguest::G((uint32_t)(0x00177f760 + iVar13)));
      (*(uint32_t*)rtguest::G((uint32_t)(0x00177ade0 + iVar9))) = uVar19;
      (*(uint32_t*)rtguest::G((uint32_t)(0x00177add0 + iVar9))) = (uint)bVar1 * 0x10 + iVar16;
      bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(iVar17 + 0x20)));
      (*(uint32_t*)rtguest::G((uint32_t)(0x00177add4 + iVar9))) = puVar12;
      (*(uint32_t*)rtguest::G((uint32_t)(0x00177ea50 +
          ((*(uint32_t*)rtguest::G((uint32_t)(0x00177ade4 + iVar9)))) * 4))) = puVar12;
      (*(uint16_t*)rtguest::G((uint32_t)(0x00177ade0 + iVar9))) =
          (*(uint16_t*)rtguest::G((uint32_t)(iVar16 + (uint)bVar1 * 0x10 + 0xe)));
    }
  }
  RT_CALLRA(0x2186d0, guest::LoadCarPerformanceSpecs(__s, param_2));
  (*(uint32_t*)rtguest::G((uint32_t)(0x00177ae8c + iVar9))) = 0x40000;
  RT_CALLRA(0x2186f4, guest::CarAiInit(__s, param_2, uVar11,
            (int)(*(uint32_t*)rtguest::G((uint32_t)(iVar18 + 0x1c))) >> 10 & 0x1f));
  RT_CALLRA(0x2186fc, guest::vu0UnitMatrix(__s));
  RT_CALLRA(0x218704, guest::vu0UnitMatrix(iVar9 + 0x00177ad00));
  RT_CALLRA(0x21870c, guest::vu0UnitMatrix(iVar9 + 0x00177ac90));
  iVar13 = (*(int32_t*)rtguest::G((uint32_t)(0x00177add4 + iVar9)));
  if (((*(uint16_t*)rtguest::G((uint32_t)(iVar13 + 8))) & 0x400) == 0) {
    iVar16 = (*(int32_t*)rtguest::G((uint32_t)(iVar18 + 0x1c)));
  }
  else {
    (*(uint32_t*)rtguest::G((uint32_t)(0x00177acc4 + iVar9))) = (*(uint32_t*)rtguest::G(0x00332b7c));
    iVar16 = (*(int32_t*)rtguest::G((uint32_t)(iVar18 + 0x1c)));
  }
  uVar14 = (uint)iVar16 >> 10;
  (*(uint8_t*)rtguest::G((uint32_t)(0x00177ae97 + iVar9))) = (byte)uVar14 & 0x1f;
  cVar10 = (*(int8_t*)rtguest::G((uint32_t)(iVar17 + 0x22)));
  if ((cVar10 == -1) || (cVar10 == '\"')) {
    iVar16 = (*(int32_t*)rtguest::G((uint32_t)(0x00177ade4 + iVar9)));
    bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(0x00177ae97 + iVar9)));
    if ((iVar16 == 0) && (bVar1 < 0x16)) {
      RT_CALLRA(0x218d70, guest::SpawnCarAtDoor(param_2, __s));
      uVar14 = (uint)(ushort)(*(int16_t*)rtguest::G((uint32_t)(0x00177ade8 + iVar9)));
    }
    else {
      if (bVar1 < 0x17) {
        if (bVar1 == 0x16) {
          bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(0x00177ae97 + iVar9)));
          if (iVar16 < 3) {
            uVar14 = (uint)(*(uint16_t*)rtguest::G((uint32_t)(0x002be43c + (uint)bVar1 * 8)));
            puVar15 = (*(uint32_t*)rtguest::G((uint32_t)(0x002c2e78 + (uint)bVar1 * 4))) + iVar16 * 0x10;
            (*(uint32_t*)rtguest::G((uint32_t)(0x00177ace0 + iVar9))) = (*(uint32_t*)rtguest::G((uint32_t)puVar15));
            uVar4 = (*(uint16_t*)rtguest::G((uint32_t)(puVar15 + 0xc)));
            iVar13 = ((uVar14 & 0x3f) >> 4) * 2 + (uVar14 & 1);
            (*(uint32_t*)rtguest::G((uint32_t)(0x00177ace4 + iVar9))) = (*(uint32_t*)rtguest::G((uint32_t)(puVar15 + 4)));
            (*(uint16_t*)rtguest::G((uint32_t)(0x00177ae24 + iVar9))) = uVar4;
            uVar19 = (*(uint32_t*)rtguest::G((uint32_t)(puVar15 + 8)));
          }
          else {
            uVar14 = (uint)(*(uint16_t*)rtguest::G((uint32_t)(0x002be43c + (uint)bVar1 * 8)));
            puVar15 = (*(uint32_t*)rtguest::G((uint32_t)(0x002c2e78 + (uint)bVar1 * 4))) + (iVar16 + -1) * 0x10;
            (*(uint32_t*)rtguest::G((uint32_t)(0x00177ace0 + iVar9))) = (*(uint32_t*)rtguest::G((uint32_t)puVar15));
            uVar4 = (*(uint16_t*)rtguest::G((uint32_t)(puVar15 + 0xc)));
            iVar13 = ((uVar14 & 0x3f) >> 4) * 2 + (uVar14 & 1);
            (*(uint32_t*)rtguest::G((uint32_t)(0x00177ace4 + iVar9))) = (*(uint32_t*)rtguest::G((uint32_t)(puVar15 + 4)));
            (*(uint16_t*)rtguest::G((uint32_t)(0x00177ae24 + iVar9))) = uVar4;
            uVar19 = (*(uint32_t*)rtguest::G((uint32_t)(puVar15 + 8)));
          }
          (*(uint32_t*)rtguest::G((uint32_t)(0x00177acf8 + iVar9))) = iVar13 * 0x2000000;
          (*(uint32_t*)rtguest::G((uint32_t)(0x00177ace8 + iVar9))) = uVar19;
          (*(uint32_t*)rtguest::G((uint32_t)(0x00177acf0 + iVar9))) = (uVar14 & 0xf) << 0x18;
          uVar8 = 5;
        }
        else {
          uVar14 = (uint)(*(uint16_t*)rtguest::G((uint32_t)(0x002be43c +
              (uint)(byte)(*(uint8_t*)rtguest::G((uint32_t)(0x00177ae97 + iVar9))) * 8)));
          puVar15 = (*(uint32_t*)rtguest::G((uint32_t)(0x002c2e78 +
              (uint)(byte)(*(uint8_t*)rtguest::G((uint32_t)(0x00177ae97 + iVar9))) * 4))) +
              (iVar16 + -4) * 0x10;
          uVar4 = (*(uint16_t*)rtguest::G((uint32_t)(puVar15 + 0xc)));
          (*(uint32_t*)rtguest::G((uint32_t)(0x00177ace0 + iVar9))) = (*(uint32_t*)rtguest::G((uint32_t)puVar15));
          (*(uint16_t*)rtguest::G((uint32_t)(0x00177ae24 + iVar9))) = uVar4;
          uVar5 = (*(uint16_t*)rtguest::G((uint32_t)(iVar13 + 8)));
          (*(uint32_t*)rtguest::G((uint32_t)(0x00177ace4 + iVar9))) = (*(uint32_t*)rtguest::G((uint32_t)(puVar15 + 4)));
          uVar19 = (*(uint32_t*)rtguest::G((uint32_t)(puVar15 + 8)));
          (*(uint32_t*)rtguest::G((uint32_t)(0x00177acf8 + iVar9))) =
              (((uVar14 & 0x3f) >> 4) * 2 + (uVar14 & 1)) * 0x2000000;
          (*(uint32_t*)rtguest::G((uint32_t)(0x00177ace8 + iVar9))) = uVar19;
          (*(uint32_t*)rtguest::G((uint32_t)(0x00177acf0 + iVar9))) = (uVar14 & 0xf) << 0x18;
          if ((uVar5 & 0x100) == 0) {
            uVar8 = 0x14;
          }
          else {
            uVar8 = 0x28;
          }
        }
      }
      else {
        bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(0x00177ae97 + iVar9)));
        if (iVar16 < 3) {
          uVar14 = (uint)(*(uint16_t*)rtguest::G((uint32_t)(0x002be43c + (uint)bVar1 * 8)));
          puVar15 = (*(uint32_t*)rtguest::G((uint32_t)(0x002c2e78 + (uint)bVar1 * 4))) + iVar16 * 0x10;
          (*(uint32_t*)rtguest::G((uint32_t)(0x00177ace0 + iVar9))) = (*(uint32_t*)rtguest::G((uint32_t)puVar15));
          uVar4 = (*(uint16_t*)rtguest::G((uint32_t)(puVar15 + 0xc)));
          iVar13 = ((uVar14 & 0x3f) >> 4) * 2 + (uVar14 & 1);
          (*(uint32_t*)rtguest::G((uint32_t)(0x00177ace4 + iVar9))) = (*(uint32_t*)rtguest::G((uint32_t)(puVar15 + 4)));
          (*(uint16_t*)rtguest::G((uint32_t)(0x00177ae24 + iVar9))) = uVar4;
          uVar19 = (*(uint32_t*)rtguest::G((uint32_t)(puVar15 + 8)));
        }
        else {
          uVar14 = (uint)(*(uint16_t*)rtguest::G((uint32_t)(0x002be43c + (uint)bVar1 * 8)));
          puVar15 = (*(uint32_t*)rtguest::G((uint32_t)(0x002c2e78 + (uint)bVar1 * 4))) + (iVar16 + -3) * 0x10;
          (*(uint32_t*)rtguest::G((uint32_t)(0x00177ace0 + iVar9))) = (*(uint32_t*)rtguest::G((uint32_t)puVar15));
          uVar4 = (*(uint16_t*)rtguest::G((uint32_t)(puVar15 + 0xc)));
          iVar13 = ((uVar14 & 0x3f) >> 4) * 2 + (uVar14 & 1);
          (*(uint32_t*)rtguest::G((uint32_t)(0x00177ace4 + iVar9))) = (*(uint32_t*)rtguest::G((uint32_t)(puVar15 + 4)));
          (*(uint16_t*)rtguest::G((uint32_t)(0x00177ae24 + iVar9))) = uVar4;
          uVar19 = (*(uint32_t*)rtguest::G((uint32_t)(puVar15 + 8)));
        }
        (*(uint32_t*)rtguest::G((uint32_t)(0x00177acf8 + iVar9))) = iVar13 * 0x2000000;
        (*(uint32_t*)rtguest::G((uint32_t)(0x00177ace8 + iVar9))) = uVar19;
        (*(uint32_t*)rtguest::G((uint32_t)(0x00177acf0 + iVar9))) = (uVar14 & 0xf) << 0x18;
        uVar8 = 0xf;
      }
      (*(uint8_t*)rtguest::G((uint32_t)(0x00177ae96 + iVar9))) = uVar8;
      uVar14 = (uint)(ushort)(*(int16_t*)rtguest::G((uint32_t)(0x00177ade8 + iVar9)));
      (*(uint8_t*)rtguest::G((uint32_t)(0x00177adea + iVar9))) = 1;
    }
    goto LAB_00218d7c;
  }
  fVar20 = (*(float*)rtguest::G((uint32_t)(0x002a7f28 + cVar10 * 0x10)));
  (*(uint32_t*)rtguest::G((uint32_t)(0x00177ace0 + iVar9))) = rtguest::f2bits(fVar20);
  (*(uint32_t*)rtguest::G((uint32_t)(0x00177ace4 + iVar9))) =
      (*(uint32_t*)rtguest::G((uint32_t)(0x002a7f2c + (*(int8_t*)rtguest::G((uint32_t)(iVar17 + 0x22))) * 0x10)));
  (*(uint32_t*)rtguest::G((uint32_t)(0x00177ace8 + iVar9))) =
      (*(uint32_t*)rtguest::G((uint32_t)(0x002a7f30 + (*(int8_t*)rtguest::G((uint32_t)(iVar17 + 0x22))) * 0x10)));
  iVar13 = ((int)(*(int16_t*)rtguest::G((uint32_t)(0x002a7f34 +
               (*(int8_t*)rtguest::G((uint32_t)(iVar17 + 0x22))) * 0x10))) << 0x1e) >> 0x10;
  (*(uint16_t*)rtguest::G((uint32_t)(0x00177ae24 + iVar9))) =
      (short)((int)(*(int16_t*)rtguest::G((uint32_t)(0x002a7f34 +
               (*(int8_t*)rtguest::G((uint32_t)(iVar17 + 0x22))) * 0x10))) << 0xe);
  if (iVar13 == -0x4000) {
    bVar1 = (byte)(*(int8_t*)rtguest::G((uint32_t)(iVar17 + 0x22)));
    if ((*(int8_t*)rtguest::G((uint32_t)(iVar17 + 0x22))) < '\x12') {
      (*(uint32_t*)rtguest::G((uint32_t)(0x00177ace0 + iVar9))) = rtguest::f2bits(
          fVar20 + (float)(byte)(*(uint8_t*)rtguest::G((uint32_t)(0x00177ae97 + iVar9))) * 5.0f);
      bVar1 = (byte)(*(int8_t*)rtguest::G((uint32_t)(iVar17 + 0x22)));
    }
    bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(0x00177ae97 + iVar9)));
    if ((*(int16_t*)rtguest::G((uint32_t)(0x002a7f36 + ((int)((uint)bVar1 << 0x18) >> 0x14)))) == 0) {
LAB_00218a10:
      fVar20 = (*(float*)rtguest::G((uint32_t)(0x00177ace8 + iVar9))) - (float)(bVar2 & 1) * 7.5f;
    }
    else {
      fVar20 = (*(float*)rtguest::G((uint32_t)(0x00177ace8 + iVar9))) + (float)(bVar2 & 1) * 7.5f;
    }
    goto LAB_00218a34;
  }
  if (iVar13 < -0x3fff) {
    if (iVar13 == -0x8000) {
      if ((*(int16_t*)rtguest::G((uint32_t)(0x002a7f36 +
          (*(int8_t*)rtguest::G((uint32_t)(iVar17 + 0x22))) * 0x10))) == 0) {
        fVar20 = fVar20 - (float)(uVar14 & 1) * 7.5f;
      }
      else {
        fVar20 = fVar20 + (float)(uVar14 & 1) * 7.5f;
      }
      (*(uint32_t*)rtguest::G((uint32_t)(0x00177ace0 + iVar9))) = rtguest::f2bits(fVar20);
      if ((*(int8_t*)rtguest::G((uint32_t)(iVar17 + 0x22))) < '\x12') {
        fVar20 = (float)rtguest::fbits((*(uint32_t*)rtguest::G((uint32_t)(0x00177ace8 + iVar9)))) +
                 (float)(byte)(*(uint8_t*)rtguest::G((uint32_t)(0x00177ae97 + iVar9))) * 5.0f;
        goto LAB_00218a34;
      }
    }
  }
  else if (iVar13 == 0) {
    if ((*(int16_t*)rtguest::G((uint32_t)(0x002a7f36 +
        (*(int8_t*)rtguest::G((uint32_t)(iVar17 + 0x22))) * 0x10))) == 0) {
      fVar20 = fVar20 + (float)(uVar14 & 1) * 7.5f;
    }
    else {
      fVar20 = fVar20 - (float)(uVar14 & 1) * 7.5f;
    }
    (*(uint32_t*)rtguest::G((uint32_t)(0x00177ace0 + iVar9))) = rtguest::f2bits(fVar20);
    if ((*(int8_t*)rtguest::G((uint32_t)(iVar17 + 0x22))) < '\x12') {
      fVar20 = (float)rtguest::fbits((*(uint32_t*)rtguest::G((uint32_t)(0x00177ace8 + iVar9)))) +
               (float)(int)-(uint)(byte)(*(uint8_t*)rtguest::G((uint32_t)(0x00177ae97 + iVar9))) * 5.0f;
      goto LAB_00218a34;
    }
  }
  else if (iVar13 == 0x4000) {
    bVar1 = (byte)(*(int8_t*)rtguest::G((uint32_t)(iVar17 + 0x22)));
    if ((*(int8_t*)rtguest::G((uint32_t)(iVar17 + 0x22))) < '\x12') {
      (*(uint32_t*)rtguest::G((uint32_t)(0x00177ace0 + iVar9))) = rtguest::f2bits(
          fVar20 + (float)(int)-(uint)(byte)(*(uint8_t*)rtguest::G((uint32_t)(0x00177ae97 + iVar9))) * 5.0f);
      bVar1 = (byte)(*(int8_t*)rtguest::G((uint32_t)(iVar17 + 0x22)));
    }
    bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(0x00177ae97 + iVar9)));
    if ((*(int16_t*)rtguest::G((uint32_t)(0x002a7f36 + ((int)((uint)bVar1 << 0x18) >> 0x14)))) != 0)
        goto LAB_00218a10;
    fVar20 = (*(float*)rtguest::G((uint32_t)(0x00177ace8 + iVar9))) + (float)(bVar2 & 1) * 7.5f;
LAB_00218a34:
    (*(uint32_t*)rtguest::G((uint32_t)(0x00177ace8 + iVar9))) = rtguest::f2bits(fVar20);
  }
  RT_CALLRA(0x00218a4c, rtCall((*(uint32_t*)rtguest::G(0x00335710)), 0x00177ace0 + iVar9, auStack_30, auStack_2c, 2,
         param_2));
  uVar14 = (uint)(ushort)(*(int16_t*)rtguest::G((uint32_t)(0x00177ade8 + iVar9)));
  if ((((*(uint16_t*)rtguest::G((uint32_t)(0x00177ade8 + iVar9))) & 0x33) == 0) &&
      ((*(int8_t*)rtguest::G((uint32_t)(0x00177add0 + iVar9))) != '\0')) {
    cVar10 = (*(uint8_t*)rtguest::G(0x00177fdd4)) != '\0';
    if ((*(uint8_t*)rtguest::G(0x00177fdd6)) != '\0') {
      cVar10 = (char)((*(uint8_t*)rtguest::G(0x00177fdd4)) != '\0') + '\x01';
    }
    if ((*(int8_t*)rtguest::G((uint32_t)(0x00177ae97 + iVar9))) != cVar10) {
      (*(uint8_t*)rtguest::G((uint32_t)(0x00177ae96 + iVar9))) =
          (char)(*(int8_t*)rtguest::G((uint32_t)(0x00177add0 + iVar9))) +
          (cVar10 - (*(int8_t*)rtguest::G((uint32_t)(0x00177ae97 + iVar9)))) + '\x01';
    }
  }
LAB_00218d7c:
  fVar21 = (*(float*)rtguest::G((uint32_t)(0x00177ace0 + iVar9))) *
           (*(float*)rtguest::G(0x00332b80));
  fVar20 = (*(float*)rtguest::G((uint32_t)(0x00177ace8 + iVar9))) *
           (*(float*)rtguest::G(0x00332b80));
  (*(int32_t*)rtguest::G((uint32_t)(0x00177acf4 + iVar9))) =
      (int)((*(float*)rtguest::G((uint32_t)(0x00177ace4 + iVar9))) * (*(float*)rtguest::G(0x00332b80)));
  (*(uint32_t*)rtguest::G((uint32_t)(0x00177acf0 + iVar9))) =
      (*(uint32_t*)rtguest::G((uint32_t)(0x00177acf0 + iVar9))) + (int)fVar21;
  (*(uint32_t*)rtguest::G((uint32_t)(0x00177acf8 + iVar9))) =
      (*(uint32_t*)rtguest::G((uint32_t)(0x00177acf8 + iVar9))) + (int)fVar20;
  // auVar7 = _sqc2(in_vf0): EE-side sqc2 of VU0 VF0.
  sqc2((uint32_t)(0x00177ad40 + iVar9), S.vf[0]);
  sVar3 = (*(int16_t*)rtguest::G((uint32_t)(0x00177ae5e + iVar9)));
  (*(uint8_t*)rtguest::G((uint32_t)(0x00177ae4f + iVar9))) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)(0x00177ae18 + iVar9))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(0x00177ae14 + iVar9))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(0x00177ae10 + iVar9))) = 0;
  (*(uint16_t*)rtguest::G((uint32_t)(0x00177ae20 + iVar9))) = 0;
  (*(uint8_t*)rtguest::G((uint32_t)(0x00177ae4e + iVar9))) = 0;
  (*(uint16_t*)rtguest::G((uint32_t)(0x00177ae1e + iVar9))) = 0;
  (*(uint16_t*)rtguest::G((uint32_t)(0x00177ae22 + iVar9))) = 0;
  if ((sVar3 != 0) && (((int)(short)uVar14 & 3U) != 0)) {
    RT_CALLRA(0x218e18, guest::PlayUiSound(((int)(short)uVar14 & 1U) + 0x8010));
    uVar14 = (uint)(ushort)(*(int16_t*)rtguest::G((uint32_t)(0x00177ade8 + iVar9)));
  }
  (*(uint32_t*)rtguest::G((uint32_t)(0x00177ae2c + iVar9))) = 0x1000;
  (*(uint32_t*)rtguest::G((uint32_t)(0x00335728 + (uVar14 & 1) * 4))) = 0;
  (*(uint16_t*)rtguest::G((uint32_t)(0x00177ae5e + iVar9))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(0x00177ae58 + iVar9))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(0x00177ae40 + iVar9))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(0x00177ae3c + iVar9))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(0x00177ae38 + iVar9))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(0x00177ae34 + iVar9))) = 0x1000;
  (*(uint32_t*)rtguest::G((uint32_t)(0x00177ae30 + iVar9))) = 0x1000;
  (*(uint32_t*)rtguest::G((uint32_t)(0x00177ad5c + iVar9))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(0x00177ad58 + iVar9))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(0x00177ad54 + iVar9))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(0x00177ad50 + iVar9))) = 0;
  if ((uVar14 & 3) == 0) {
    (*(uint16_t*)rtguest::G((uint32_t)(0x00177ae90 + iVar9))) = 0;
    cVar10 = (*(int8_t*)rtguest::G((uint32_t)(iVar17 + 10)));
  }
  else {
    RT_CALLRA(0x218e70, guest::SoundRpcPlaySequence(uVar14 & 1));
    cVar10 = (*(int8_t*)rtguest::G((uint32_t)(iVar17 + 10)));
  }
  if (cVar10 == '\b') {
    (*(uint8_t*)rtguest::G((uint32_t)(0x00177adea + iVar9))) = 1;
  }
  (*(uint32_t*)rtguest::G((uint32_t)(iVar18 + 0xc))) = 0x0021ba70;   // task update fn
  (*(uint64_t*)rtguest::G((uint32_t)(iVar18 + 0x28))) = __s;
  (*(uint32_t*)rtguest::G((uint32_t)(iVar18 + 0x2c))) =
      0x0021cbb8;  // 3-insn leaf: *(ushort*)(*(u32*)(a0+0x28)+0x198)=0; W12 must register it
  RT_CALLRA(0x218ebc, guest::UpdateVehiclePhysics(param_1, param_2));
  return;
}


RT_GUEST_FN(0x00218458, guest::SpawnCarEntityTask)

}  // namespace guest
