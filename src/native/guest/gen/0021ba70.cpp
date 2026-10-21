// guest:0x0021ba70 transliterated from Ghidra pseudocode + ELF disassembly.
// Ghidra's stack scalars iStack_2e0..uStack_2d4 / uStack_2d0..uStack_2c4 are two
// contiguous 16-byte fixed-point vectors (address-taken by callees) -> RAM slots.
// CONCAT44(s>>0xf, s)==0/k==2 checks are direct comparisons on the short.
// Ghidra double literals in float context render the guest single-precision
// constants (0.0, 300.0, -3.0517578e-05 == -2^-15 exactly) -> float suffixes.
#include "../guest_vu0.h"
#include "guest_decls.inc"

namespace guest {

void UpdateVehiclePhysics(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(736);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x2c8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x2cc) = 0x00000000u;
  }
  using namespace rtguest::vu0;
  int iVar1;
  ushort uVar2;
  int iVar3;
  int iVar4;
  bool bVar5;
  uint64_t uVar6;
  uint64_t uVar7;
  char cVar9;
  char cVar10;
  byte bVar11;
  int iVar12;
  uint uVar13;
  int iVar14;
  int iVar15;
  long lVar16;
  short sVar17;
  uint32_t uVar18;
  uint32_t uVar19;
  uint32_t pauVar20;
  int iVar21;
  uint32_t puVar22;
  uint32_t pauVar23;
  int iVar24;
  float fVar25;
  float fVar26;
  uint32_t vecA = rtguest::allocFrame(16);   // iStack_2e0,uStack_2dc,iStack_2d8,uStack_2d4
  uint32_t vecB = rtguest::allocFrame(16);   // uStack_2d0,uStack_2cc,uStack_2c8,uStack_2c4
  uint32_t auStack_2c0 = rtguest::allocFrame(64);
  uint32_t auStack_280 = rtguest::allocFrame(64);
  uint32_t auStack_240 = rtguest::allocFrame(64);
  uint32_t auStack_200 = rtguest::allocFrame(64);
  uint32_t auStack_1c0 = rtguest::allocFrame(16);  // + uStack_1bc (word at +4)
  uint32_t auStack_1b0 = rtguest::allocFrame(32);
  uint32_t auStack_190 = rtguest::allocFrame(64);
  uint32_t auStack_150 = rtguest::allocFrame(64);
  int iStack_110;
  int iStack_10c;
  int iStack_108;
  uint32_t auStack_100 = rtguest::allocFrame(64);
  uint32_t auStack_c0 = rtguest::allocFrame(64);
  uint32_t afStack_80 = rtguest::allocFrame(16);   // 3 floats + uStack_74
  int iStack_70;

  iVar24 = (int)param_2;
  if (((*(uint32_t*)rtguest::G((uint32_t)(iVar24 + 0x28))) & 0x4a000) != 0) {
    return;
  }
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)(param_1 + 0x28)));
  sVar17 = (*(int16_t*)rtguest::G((uint32_t)(iVar3 + 0x198)));
  if (sVar17 == 0) {  // CONCAT44(sign-ext,s)==0 iff s==0
    return;
  }
  if (((*(uint32_t*)rtguest::G((uint32_t)(iVar24 + 0x28))) & 0x400) == 0) {
    (*(uint16_t*)rtguest::G((uint32_t)(iVar3 + 0x198))) =
        (*(uint16_t*)rtguest::G((uint32_t)(iVar3 + 0x198))) & 0xfeff;
LAB_0021bc00:
    cVar9 = (*(int8_t*)rtguest::G((uint32_t)(iVar24 + 0xb)));
  }
  else {
    if (sVar17 != 2) goto LAB_0021bc00;
    (*(uint16_t*)rtguest::G((uint32_t)(iVar3 + 0x198))) =
        (*(uint16_t*)rtguest::G((uint32_t)(iVar3 + 0x198))) | 0x100;
    (*(uint32_t*)rtguest::G((uint32_t)(param_1 + 8))) = 0;
    RT_CALLRA(0x21baf4, guest::SpawnCarAtDoor(param_2, iVar3));
    fVar25 = (*(float*)rtguest::G((uint32_t)(iVar3 + 0x90))) *
             (*(float*)rtguest::G(0x00332c08));
    fVar26 = (*(float*)rtguest::G((uint32_t)(iVar3 + 0x98))) *
             (*(float*)rtguest::G(0x00332c08));
    (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0xa4))) =
        (int)((*(float*)rtguest::G((uint32_t)(iVar3 + 0x94))) *
              (*(float*)rtguest::G(0x00332c08)));
    (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0xa0))) =
        (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0xa0))) + (int)fVar25;
    (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0xa8))) =
        (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0xa8))) + (int)fVar26;
    RT_CALLRA(0x21bb4c, guest::LoadCarPerformanceSpecs(iVar3, param_2));
    // auVar8 = _sqc2(in_vf0): EE-side sqc2 of VU0 VF0.
    sqc2((uint32_t)(iVar3 + 0xf0), S.vf[0]);
    (*(uint8_t*)rtguest::G((uint32_t)(iVar3 + 0x1ff))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x1c8))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x1c4))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x1c0))) = 0;
    (*(uint16_t*)rtguest::G((uint32_t)(iVar3 + 0x1d0))) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)(iVar3 + 0x1fe))) = 0;
    (*(uint16_t*)rtguest::G((uint32_t)(iVar3 + 0x1ce))) = 0;
    (*(uint16_t*)rtguest::G((uint32_t)(iVar3 + 0x1d2))) = 0;
    if ((*(int16_t*)rtguest::G((uint32_t)(iVar3 + 0x20e))) == 0) {
LAB_0021bba8:
      uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(iVar3 + 0x198)));
    }
    else {
      uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(iVar3 + 0x198)));
      if (((*(uint16_t*)rtguest::G((uint32_t)(iVar3 + 0x198))) & 3) != 0) {
        RT_CALLRA(0x21bba0, guest::PlayUiSound(((int)(short)(*(uint16_t*)rtguest::G((uint32_t)(iVar3 + 0x198))) & 1U) + 0x8010));
        goto LAB_0021bba8;
      }
    }
    (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x1dc))) = 0x1000;
    (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x208))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x1f0))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x1ec))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x1e8))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x1e4))) = 0x1000;
    (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x1e0))) = 0x1000;
    (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x10c))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x108))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x104))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x100))) = 0;
    (*(uint16_t*)rtguest::G((uint32_t)(iVar3 + 0x20e))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(0x00335728 + (uVar2 & 1) * 4))) = 0;
    cVar9 = (*(int8_t*)rtguest::G((uint32_t)(iVar24 + 0xb)));
  }
  pauVar23 = iVar3 + 0xf0;
  (*(uint32_t*)rtguest::G(0x003356d0)) = 0;
  if (cVar9 != '\0') {
    (*(uint32_t*)rtguest::G(0x003356d0)) =
        ((*(int16_t*)rtguest::G((uint32_t)(iVar3 + 0x198))) == 2) ? 1u : 0u;
  }
  (*(uint32_t*)rtguest::G(0x003356d4)) = 0;
  if (cVar9 != '\0') {
    (*(uint32_t*)rtguest::G(0x003356d4)) =
        ((*(int16_t*)rtguest::G((uint32_t)(iVar3 + 0x198))) == 2) ? 1u : 0u;
  }
  (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x1f8))) = 0;
  if ((*(int8_t*)rtguest::G((uint32_t)(iVar3 + 0x212))) != '\0') {
    (*(int8_t*)rtguest::G((uint32_t)(iVar3 + 0x212))) =
        (int8_t)((*(int8_t*)rtguest::G((uint32_t)(iVar3 + 0x212))) + -1);
  }
  if ((*(int16_t*)rtguest::G((uint32_t)(iVar3 + 0x210))) != 0) {
    (*(int16_t*)rtguest::G((uint32_t)(iVar3 + 0x210))) =
        (short)((*(int16_t*)rtguest::G((uint32_t)(iVar3 + 0x210))) + -1);
  }
  pauVar20 = iVar3 + 0x100;
  iStack_70 = (int)(*(int16_t*)rtguest::G((uint32_t)(iVar3 + 0x1d4)));
  if ((*(uint32_t*)rtguest::G(0x003356d0)) != 0) {
    ::sprintf((char *)rtguest::G((uint32_t)(auStack_2c0)),
              (char *)rtguest::G((uint32_t)(0x002ead90)), 0x00333598,
              (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x108))),
              (*(uint32_t*)rtguest::G((uint32_t)pauVar20)),
              (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x104))),
              (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x10c))));
    RT_CALLRA(0x21bca8, guest::uiDebugText8(300, 0x10, auStack_2c0, 0xf));
  }
  // EE PSUBSW: 8x saturated signed 16-bit sub (quad at car+0xf0 - quad at car+0x100).
  {
    const uint8_t *pa = (const uint8_t *)rtguest::G((uint32_t)(iVar3 + 0xf0));
    const uint8_t *pb = (const uint8_t *)rtguest::G((uint32_t)(iVar3 + 0x100));
    for (int k = 0; k < 4; ++k) {
      int lo = (int)(int16_t)((uint16_t)pa[k * 4] | (uint16_t)(pa[k * 4 + 1] << 8)) -
               (int)(int16_t)((uint16_t)pb[k * 4] | (uint16_t)(pb[k * 4 + 1] << 8));
      int hi = (int)(int16_t)((uint16_t)pa[k * 4 + 2] | (uint16_t)(pa[k * 4 + 3] << 8)) -
               (int)(int16_t)((uint16_t)pb[k * 4 + 2] | (uint16_t)(pb[k * 4 + 3] << 8));
      if (lo > 0x7fff) lo = 0x7fff;
      else if (lo < -0x8000) lo = -0x8000;
      if (hi > 0x7fff) hi = 0x7fff;
      else if (hi < -0x8000) hi = -0x8000;
      (*(uint32_t*)rtguest::G((uint32_t)(vecB + k * 4))) =
          (uint32_t)((uint16_t)lo | ((uint32_t)(uint16_t)hi << 16));
    }
  }
  iVar21 = iVar3 + 0xb0;
  RT_CALLRA(0x21bcd0, guest::TransformVecByF15Matrix(vecB, iVar21, vecB));
  if ((*(uint32_t*)rtguest::G(0x003356d0)) != 0) {
    ::sprintf((char *)rtguest::G((uint32_t)(auStack_280)),
              (char *)rtguest::G((uint32_t)(0x002ead90)), 0x003335a0,
              (*(uint32_t*)rtguest::G((uint32_t)(vecB + 8))),
              (*(uint32_t*)rtguest::G((uint32_t)vecB)),
              (*(uint32_t*)rtguest::G((uint32_t)(vecB + 4))),
              (*(uint32_t*)rtguest::G((uint32_t)(vecB + 0xc))));
    RT_CALLRA(0x21bd14, guest::uiDebugText8(300, 0x30, auStack_280, 0xf));
  }
  (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0xf4))) =
      (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0xf4))) + -0x59;
  uVar18 = (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0xf8)));
  uVar19 = (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0xfc)));
  (*(int32_t*)rtguest::G((uint32_t)pauVar20)) =
      (int)((*(uint64_t*)rtguest::G((uint32_t)pauVar23)));
  (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0x104))) =
      (int)((*(uint64_t*)rtguest::G((uint32_t)pauVar23)) >> 0x20);
  (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x108))) = uVar18;
  (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x10c))) = uVar19;
  if ((*(uint32_t*)rtguest::G(0x003356d0)) != 0) {
    ::sprintf((char *)rtguest::G((uint32_t)(auStack_240)),
              (char *)rtguest::G((uint32_t)(0x002ead90)), 0x003335a8,
              (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0xf8))),
              (*(uint32_t*)rtguest::G((uint32_t)pauVar23)),
              (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0xf4))),
              (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0xfc))));
    RT_CALLRA(0x21bd6c, guest::uiDebugText8(300, 0x18, auStack_240, 0xf));
  }
  RT_CALLRA(0x21bd80, guest::TransformVecByF15Matrix(vecA, iVar21, pauVar23));
  if ((*(uint32_t*)rtguest::G(0x003356d0)) != 0) {
    ::sprintf((char *)rtguest::G((uint32_t)(auStack_200)),
              (char *)rtguest::G((uint32_t)(0x002ead90)), 0x003335b0,
              (*(int32_t*)rtguest::G((uint32_t)(vecA + 8))),
              (*(int32_t*)rtguest::G((uint32_t)vecA)),
              (*(uint32_t*)rtguest::G((uint32_t)(vecA + 4))),
              (*(uint32_t*)rtguest::G((uint32_t)(vecA + 0xc))));
    RT_CALLRA(0x21bdc0, guest::uiDebugText8(300, 0x20, auStack_200, 0xf));
  }
  ::memset((char *)rtguest::G((uint32_t)(auStack_1c0)), 0, 0x10);
  (*(uint32_t*)rtguest::G((uint32_t)(auStack_1c0 + 4))) = 0x59;
  RT_CALLRA(0x21bdec, guest::TransformVecByF15Matrix(auStack_1c0, iVar21, auStack_1c0));
  (*(uint32_t*)rtguest::G((uint32_t)(vecA + 0xc))) =
      (*(uint32_t*)rtguest::G((uint32_t)(auStack_1c0 + 4)));
  uVar6 = RT_CALLRA(0x21be04, guest::AcquireVehicleInput(param_1, param_2, iVar3));
  bVar5 = 0x1944c < ((*(uint32_t*)rtguest::G((uint32_t)(iVar24 + 0x14))) - 0xe484U);
  (*(uint8_t*)rtguest::G((uint32_t)(iVar3 + 0x1fc))) = bVar5 ? 1 : 0;
  if ((uVar6 & 0x10) != 0) {
    (*(uint8_t*)rtguest::G((uint32_t)(iVar3 + 0x1fc))) = (!bVar5) ? 1 : 0;
  }
  iVar21 = (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0x184)));
  if (((*(uint16_t*)rtguest::G((uint32_t)(iVar21 + 8))) & 0x2000) != 0) {
    iVar21 = (int)RT_CALLRA(0x21be60, guest::UpdateContinuousSoundEffect(param_2, iVar3 + 0x20e, (uint32_t)uVar6, iVar3 + 0x23c,
              (*(uint16_t*)rtguest::G((uint32_t)(iVar3 + 0x198)))));
    (*(int32_t*)rtguest::G((uint32_t)(vecA + 8))) =
        (*(int32_t*)rtguest::G((uint32_t)(vecA + 8))) + iVar21;
    if ((*(int32_t*)rtguest::G((uint32_t)(vecA + 8))) < 0xcf6a) {
      iVar21 = (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0x184)));
    }
    else {
      iVar21 = (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0x184)));
      (*(int32_t*)rtguest::G((uint32_t)(vecA + 8))) = 0xcf69;
    }
  }
  cVar9 = (*(int8_t*)rtguest::G((uint32_t)(iVar3 + 0x213)));
  if ((((*(uint16_t*)rtguest::G((uint32_t)(iVar21 + 8))) & 0x40) != 0) &&
      ((*(int8_t*)rtguest::G((uint32_t)(iVar3 + 0x213))) != '\0')) {
    if ((uVar6 & 4) == 0) {
      if ((uVar6 & 1) == 0) goto LAB_0021bed8;
      (*(int32_t*)rtguest::G((uint32_t)(vecA + 8))) =
          (*(int32_t*)rtguest::G((uint32_t)(vecA + 8))) + 0x59;
    }
    else {
      (*(int32_t*)rtguest::G((uint32_t)(vecA + 8))) =
          (*(int32_t*)rtguest::G((uint32_t)(vecA + 8))) + -0x59;
    }
    cVar9 = (*(int8_t*)rtguest::G((uint32_t)(iVar3 + 0x213)));
  }
LAB_0021bed8:
  cVar10 = '\0';
  if (((*(uint8_t*)rtguest::G((uint32_t)(iVar24 + 10))) & 2) != 0) {
    cVar10 = (*(int8_t*)rtguest::G((uint32_t)(iVar3 + 0x247)));
  }
  uVar13 = 0x600;
  if (cVar10 != '\0') {
    uVar13 = 0x500;
  }
  if (cVar9 < '\0') {
    uVar13 = uVar13 >> 1;
  }
  if ('\0' < cVar9) {
    uVar13 = (uint)((int)uVar13 >> 2);
  }
  iVar14 = (int)(*(int32_t*)rtguest::G((uint32_t)(vecA + 8))) / (int)uVar13;
  if (uVar13 == 0) {
    trap(7);
  }
  iVar12 = (iVar14 * iVar14) / (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0x218)));
  if ((*(uint32_t*)rtguest::G(0x003356d4)) != 0) {
    ::sprintf((char *)rtguest::G((uint32_t)(auStack_1b0)),
              (char *)rtguest::G((uint32_t)(0x00333588)), iVar12);
    RT_CALLRA(0x21bf70, guest::uiDebugText8(0xdc, 0x30, auStack_1b0, 0xf));
    iVar21 = (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0x184)));
  }
  if (-1 < iVar14) {
    iVar12 = -iVar12;
  }
  (*(int32_t*)rtguest::G((uint32_t)(vecA + 8))) =
      (*(int32_t*)rtguest::G((uint32_t)(vecA + 8))) + iVar12;
  iVar14 = (int)(*(int32_t*)rtguest::G((uint32_t)vecA));
  if ((*(int8_t*)rtguest::G((uint32_t)(iVar3 + 0x213))) < '\x01') {
    if ((int)(*(int32_t*)rtguest::G((uint32_t)vecA)) < 0) {
      iVar14 = (int)(*(int32_t*)rtguest::G((uint32_t)vecA)) + 0x3ff;
    }
    iVar12 = iVar14 >> 10;
  }
  else {
    if ((int)(*(int32_t*)rtguest::G((uint32_t)vecA)) < 0) {
      iVar14 = (int)(*(int32_t*)rtguest::G((uint32_t)vecA)) + 0x3f;
    }
    iVar12 = iVar14 >> 6;
  }
  if ((*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0x218))) == 0) {
    trap(7);
  }
  iVar12 = (iVar12 * iVar12) / (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0x218)));
  if (-1 < (int)((uint)iVar14 >> 0x1f)) {
    iVar12 = -iVar12;
  }
  (*(int32_t*)rtguest::G((uint32_t)vecA)) =
      (int)((*(int32_t*)rtguest::G((uint32_t)vecA)) + iVar12);
  iVar12 = (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0x1c0)));
  iVar14 = (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0x218)));
  if ((*(int8_t*)rtguest::G((uint32_t)(iVar3 + 0x213))) < '\x01') {
    iVar15 = iVar12;
    if (iVar12 < 0) {
      iVar15 = iVar12 + 0x3ff;
    }
    iVar15 = iVar15 >> 10;
  }
  else {
    iVar15 = iVar12;
    if (iVar12 < 0) {
      iVar15 = iVar12 + 0x3f;
    }
    iVar15 = iVar15 >> 6;
  }
  if (iVar14 == 0) {
    trap(7);
  }
  iVar4 = (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0x1c4)));
  iVar1 = (iVar15 * iVar15) / iVar14;
  if (-1 < iVar15) {
    iVar1 = -iVar1;
  }
  (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0x1c0))) = iVar12 + iVar1;
  if ((int)((uint)(*(uint8_t*)rtguest::G((uint32_t)(iVar3 + 0x213))) << 0x18) < 1) {
    iVar12 = iVar4;
    if (iVar4 < 0) {
      iVar12 = iVar4 + 0x3ff;
    }
    iVar12 = iVar12 >> 10;
  }
  else {
    iVar12 = iVar4;
    if (iVar4 < 0) {
      iVar12 = iVar4 + 0x3f;
    }
    iVar12 = iVar12 >> 6;
  }
  if (iVar14 == 0) {
    trap(7);
  }
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0x1c8)));
  iVar15 = (iVar12 * iVar12) / iVar14;
  if (-1 < iVar12) {
    iVar15 = -iVar15;
  }
  (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0x1c4))) = iVar4 + iVar15;
  if ((int)((uint)(*(uint8_t*)rtguest::G((uint32_t)(iVar3 + 0x213))) << 0x18) < 1) {
    iVar12 = iVar1;
    if (iVar1 < 0) {
      iVar12 = iVar1 + 0x3ff;
    }
    iVar12 = iVar12 >> 10;
  }
  else {
    iVar12 = iVar1;
    if (iVar1 < 0) {
      iVar12 = iVar1 + 0x3f;
    }
    iVar12 = iVar12 >> 6;
  }
  if (iVar14 == 0) {
    trap(7);
  }
  uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(iVar21 + 8)));
  iVar14 = (iVar12 * iVar12) / iVar14;
  if (-1 < iVar12) {
    iVar14 = -iVar14;
  }
  (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0x1c8))) = iVar1 + iVar14;
  if ((uVar2 & 4) == 0) {
    if ((uVar2 & 8) == 0) {
      uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(iVar21 + 8)));
      goto LAB_0021c1b4;
    }
    fVar25 = rtguest::fbits(RT_CALLRA(0x21c138, guest::GetSpeedFromF15Vector(pauVar23)));  // callee ends mul.s $f0 / jr ra (0x21d37c): float bits
    iVar21 = (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0x184)));
    if (fVar25 < 300.0f) {
      (*(uint16_t*)rtguest::G((uint32_t)(iVar21 + 8))) =
          (*(uint16_t*)rtguest::G((uint32_t)(iVar21 + 8))) & 0xfff7 | 4;
    }
  }
  else {
    fVar25 = rtguest::fbits(RT_CALLRA(0x21c17c, guest::GetSpeedFromF15Vector(pauVar23)));  // callee ends mul.s $f0 / jr ra (0x21d37c): float bits
    iVar21 = (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0x184)));
    if (300.0f < fVar25) {
      (*(uint16_t*)rtguest::G((uint32_t)(iVar21 + 8))) =
          (*(uint16_t*)rtguest::G((uint32_t)(iVar21 + 8))) & 0xfffb | 8;
    }
  }
  uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(iVar21 + 8)));
LAB_0021c1b4:
  if ((uVar2 & 0x1000) != 0) {
    bVar11 = (*(uint8_t*)rtguest::G((uint32_t)(iVar3 + 0x244)));
    if ((-1 < (char)bVar11) && ((uVar6 & 0x20) != 0)) {
      (*(uint8_t*)rtguest::G((uint32_t)(iVar3 + 0x244))) = (byte)(bVar11 - 1);
      bVar11 = (byte)(bVar11 - 1);
    }
    if (((int)((uint)bVar11 << 0x18) < 1) && ((uVar6 & 0x40) != 0)) {
      (*(uint8_t*)rtguest::G((uint32_t)(iVar3 + 0x244))) = (byte)(bVar11 + 1);
    }
    sVar17 = 0;
    lVar16 = -1;
    if ((uVar6 & 0x8000) == 0) {
      lVar16 = 0;
    }
    if ((uVar6 & 0x2000) != 0) {
      lVar16 = 1;
    }
    if (lVar16 != 0) {
      iVar21 = (int)lVar16 * (int)(*(int32_t*)rtguest::G((uint32_t)(vecA + 8)));
      if (iVar21 < 0) {
        iVar21 = iVar21 + 0x1ff;
      }
      sVar17 = (short)(iVar21 >> 9);
    }
    (*(int16_t*)rtguest::G((uint32_t)(iVar3 + 0x1d4))) =
        (short)((*(int16_t*)rtguest::G((uint32_t)(iVar3 + 0x1d4))) + sVar17);
  }
  if ((*(uint32_t*)rtguest::G(0x003356d0)) != 0) {
    ::sprintf((char *)rtguest::G((uint32_t)(auStack_190)),
              (char *)rtguest::G((uint32_t)(0x002ead90)), 0x003335b0,
              (*(int32_t*)rtguest::G((uint32_t)(vecA + 8))),
              (*(int32_t*)rtguest::G((uint32_t)vecA)),
              (*(uint32_t*)rtguest::G((uint32_t)(vecA + 4))),
              (*(uint32_t*)rtguest::G((uint32_t)(vecA + 0xc))));
    RT_CALLRA(0x21c27c, guest::uiDebugText8(300, 0x28, auStack_190, 0xf));
  }
  if ((*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0x1c0))) == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0xf4))) = 0;
  }
  else if ((*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0x1c4))) == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0xf4))) = 0;
  }
  else if ((*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0x1c8))) == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0xf4))) = 0;
  }
  RT_CALLRA(0x21c2bc, guest::gs_controls_car_update(param_2, iVar3, vecA, (uint32_t)uVar6, vecB));
  if ((*(uint32_t*)rtguest::G(0x003356d0)) != 0) {
    ::sprintf((char *)rtguest::G((uint32_t)(auStack_150)),
              (char *)rtguest::G((uint32_t)(0x002ead90)), 0x003335a0,
              (*(uint32_t*)rtguest::G((uint32_t)(vecB + 8))),
              (*(uint32_t*)rtguest::G((uint32_t)vecB)),
              (*(uint32_t*)rtguest::G((uint32_t)(vecB + 4))),
              (*(uint32_t*)rtguest::G((uint32_t)(vecB + 0xc))));
    RT_CALLRA(0x21c300, guest::uiDebugText8(300, 0x40, auStack_150, 0xf));
  }
  iStack_110 = ((*(int32_t*)rtguest::G((uint32_t)pauVar23)) << 4) / 0x19;
  puVar22 = iVar3 + 0xa0;
  iStack_10c = ((*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0xf4))) << 4) / 0x19;
  iStack_108 = ((*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0xf8))) << 4) / 0x19;
  (*(uint32_t*)rtguest::G((uint32_t)puVar22)) =
      ((*(uint32_t*)rtguest::G((uint32_t)puVar22)) + (uint32_t)iStack_110) & 0xfffffff;
  (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0xa4))) =
      (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0xa4))) + iStack_10c;
  (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0xa8))) =
      ((*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0xa8)) + (uint32_t)iStack_108)) & 0xfffffff;
  if ((*(uint32_t*)rtguest::G(0x003356d0)) != 0) {
    ::sprintf((char *)rtguest::G((uint32_t)(auStack_100)),
              (char *)rtguest::G((uint32_t)(0x002ead90)), 0x003335a8,
              (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0xf8))),
              (*(uint32_t*)rtguest::G((uint32_t)pauVar23)),
              (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0xf4))),
              (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0xfc))));
    RT_CALLRA(0x21c3d4, guest::uiDebugText8(300, 0x50, auStack_100, 0xf));
  }
  if ((*(int8_t*)rtguest::G((uint32_t)(iVar3 + 0x213))) < '\x01') {
    iVar21 = (int)(*(int8_t*)rtguest::G((uint32_t)(iVar3 + 0x244))) *
                 (int)(*(int32_t*)rtguest::G((uint32_t)(vecA + 8))) +
             (int)(*(int32_t*)rtguest::G((uint32_t)(vecA + 8)));
    if (iVar21 < 0) {
      iVar21 = iVar21 + 0x7ff;
    }
    iVar21 = iVar21 >> 0xb;
  }
  else {
    iVar21 = (int)(*(int8_t*)rtguest::G((uint32_t)(iVar3 + 0x244))) *
             (int)(*(int32_t*)rtguest::G((uint32_t)(vecA + 8)));
    if (iVar21 < 0) {
      iVar21 = iVar21 + 0xff;
    }
    iVar21 = iVar21 >> 8;
  }
  uVar6 = RT_CALLRA(0x21c434, guest::ResolveGroundContact(param_2, iVar3, vecB, vecA, iVar21, uVar6));
  fVar25 = (*(float*)rtguest::G(0x00332c0c));
  (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x9c))) = 0x3f800000;
  (*(float*)rtguest::G((uint32_t)(iVar3 + 0x94))) =
      (float)(*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0xa4))) / fVar25;
  (*(float*)rtguest::G((uint32_t)(iVar3 + 0x98))) =
      (float)(*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0xa8)) & 0x1ffffff) / fVar25;
  (*(float*)rtguest::G((uint32_t)(iVar3 + 0x90))) =
      (float)(((*(uint32_t*)rtguest::G((uint32_t)puVar22)) +
               ((int)((*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0xa8))) & 0x2000000) >> 1)) &
              0x1ffffff) /
      fVar25;
  uVar7 = RT_CALLRA(0x21c4c8, guest::QueryCarSurfaces(param_2, iVar3));
  RT_CALLRA(0x21c4e0, guest::ApplyCollisionImpulse(param_2, iVar3, (uint32_t)(uVar6 | uVar7),
                        (int32_t)(*(int32_t*)rtguest::G((uint32_t)(vecA + 8))), iStack_70));
  if ((*(uint32_t*)rtguest::G(0x003356d0)) != 0) {
    ::sprintf((char *)rtguest::G((uint32_t)(auStack_c0)),
              (char *)rtguest::G((uint32_t)(0x002ead90)), 0x003335a8,
              (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0xf8))),
              (*(uint32_t*)rtguest::G((uint32_t)pauVar23)),
              (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0xf4))),
              (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0xfc))));
    RT_CALLRA(0x21c520, guest::uiDebugText8(300, 0x58, auStack_c0, 0xf));
  }
  (*(float*)rtguest::G((uint32_t)(afStack_80 + 4))) = 1.0f;
  (*(uint32_t*)rtguest::G((uint32_t)(afStack_80 + 0xc))) = 0;
  (*(float*)rtguest::G((uint32_t)afStack_80)) =
      (float)(*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0x1e4)) -
              (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0x1e0)))) /
      (*(float*)rtguest::G(0x00332c10));
  (*(float*)rtguest::G((uint32_t)(afStack_80 + 8))) =
      (float)((*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0x1dc))) -
              ((*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0x1e0))) +
               (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0x1e4)))) /
                  2) /
      (*(float*)rtguest::G(0x00332c14));
  RT_CALLRA(0x21c598, guest::BuildYawTransform(iVar3 + 0x40, afStack_80));
  fVar25 = (float)(((*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0x1dc))) +
                   ((*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0x1e0))) +
                    (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0x1e4)))) /
                       2) /
                       2 +
                   -0x1000) *
           -3.0517578e-05f;
  if (((*(uint16_t*)rtguest::G((uint32_t)((*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0x184))) + 8))) &
       0x400) == 0) {
    fVar25 = fVar25 + 0.0f;
  }
  else {
    fVar25 = fVar25 + (*(float*)rtguest::G(0x00332c18));
  }
  (*(float*)rtguest::G((uint32_t)(iVar3 + 0x74))) = fVar25;
  if (((*(uint16_t*)rtguest::G((uint32_t)(iVar3 + 0x198))) & 0x200) == 0) {
    iVar21 = (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0x1b8)));
    if (iVar21 < 0) {
      iVar21 = -iVar21;
    }
    (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0x204))) =
        (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0x204))) + iVar21;
  }
  RT_CALLRA(0x21c63c, guest::UpdateSuspensionFlex(iVar3));
  if (((*(int8_t*)rtguest::G((uint32_t)(iVar24 + 0xb))) != '\0') &&
      (((*(uint16_t*)rtguest::G((uint32_t)(iVar3 + 0x198))) & 3) == 2)) {
    RT_CALLRA(0x21c660, guest::DrawRaceHudNumbers(iVar3));
  }
  return;
}


RT_GUEST_FN(0x0021ba70, guest::UpdateVehiclePhysics)

}  // namespace guest
