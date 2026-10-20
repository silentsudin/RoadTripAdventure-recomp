// guest:0x00259a08 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t UpdateNpcTailCarTransform(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x58) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x5c) = 0x00000000u;
  }
    // alloc order = guest stack layout: &uStack_60 spans _60,_5c,_58 ascending
    // (TransformRectangle writes float3), auStack_50 adjacent above.
    uint32_t uStack_60 = rtguest::allocFrame(4);
    uint32_t uStack_5c = rtguest::allocFrame(4);
    uint32_t uStack_58 = rtguest::allocFrame(4);
uint32_t pbVar1;
  int iVar2;
  byte bVar3;
  byte bVar4;
  int iVar5;
  short sVar6;
  byte bVar7;
uint32_t pfVar8;
  int iVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  float fVar13;
  float fVar14;
  float fVar15;
uint32_t auStack_50 = rtguest::allocFrame(16);
  
  pbVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0x8000) != 0) goto LAB_0025a078;
  pfVar8 = (uint32_t)(pbVar1 + 0x20);
  RT_CALLRA(0x259a5c, guest::vu0SubVector(auStack_50,pfVar8,0x177ace0));
  fVar10 = rtguest::fbits(RT_CALLRA(0x259a68, guest::vecDotProductVu0(auStack_50,auStack_50)));
  if (SQRT(fVar10) < 1.5) {
    if (((*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(2) * sizeof(int8_t))) & 1) == 0) {
      (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(6) * sizeof(int8_t))) = 0;
      (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(7) * sizeof(int8_t))) = 0;
    }
    bVar3 = 0;
    if (((*(float*)rtguest::G((uint32_t)pfVar8)) - (*(float*)rtguest::G(0x3330b8)) <= (*(float*)rtguest::G(0x177ace0))) &&
       (bVar3 = 1, (*(float*)rtguest::G(0x177ace0)) <= (*(float*)rtguest::G((uint32_t)pfVar8)) + (*(float*)rtguest::G(0x3330b8)))) {
      bVar3 = 2;
      if (((*(float*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x28))) - (*(float*)rtguest::G(0x3330b8)) <= (*(float*)rtguest::G(0x177ace8))) &&
         (bVar3 = 3, (*(float*)rtguest::G(0x177ace8)) <= (*(float*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x28))) + (*(float*)rtguest::G(0x3330b8)))) {
        bVar3 = 0;
      }
    }
    (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(3) * sizeof(int8_t))) = bVar3;
    fVar11 = rtguest::fbits(RT_CALLRA(0x259b3c, guest::GetSpeedFromF15Vector(0x177ad40)));
    iVar2 = (*(int32_t*)rtguest::G(0x177ad48));
    fVar10 = (*(float*)rtguest::G(0x3330c0));
    iVar9 = (int)fVar11;
    iVar5 = (*(int32_t*)rtguest::G(0x177ad40)) / 0x98;
    fVar11 = (float)iVar9 * (*(float*)rtguest::G(0x3330bc));
    (*(float*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x18))) = fVar11;
    (*(float*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0xc))) = fVar11;
    fVar11 = (float)iVar5 * fVar10;
    (*(float*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x14))) = fVar11;
    (*(float*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 8))) = fVar11;
    fVar10 = (float)(iVar2 / 0x98) * fVar10;
    (*(float*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x1c))) = fVar10;
    (*(float*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x10))) = fVar10;
    if ((*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(2) * sizeof(int8_t))) == 0) {
      iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) = 1;
    }
    else {
      iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) = 2;
    }
    if (iVar9 - 1U < 0xe) {
      bVar3 = (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(2) * sizeof(int8_t))) | 9;
LAB_00259c20:
      (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(2) * sizeof(int8_t))) = bVar3;
      bVar3 = (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(4) * sizeof(int8_t)));
    }
    else {
      if (0xe < iVar9) {
        bVar3 = (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(2) * sizeof(int8_t))) | 0xe;
        goto LAB_00259c20;
      }
      bVar3 = (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(4) * sizeof(int8_t)));
    }
    if (bVar3 == 0) {
      bVar3 = (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(2) * sizeof(int8_t)));
    }
    else {
      (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(2) * sizeof(int8_t))) = (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(2) * sizeof(int8_t))) & 0xfd;
      bVar3 = (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(2) * sizeof(int8_t)));
    }
  }
  else {
    bVar3 = (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(2) * sizeof(int8_t)));
  }
  if ((bVar3 & 1) != 0) {
    bVar7 = (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(3) * sizeof(int8_t)));
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 6))) & 7) < 4) {
      if (bVar7 == 0) {
        (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x34))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x34))) + -0x80;
      }
      if (bVar7 == 1) {
        (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x34))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x34))) + 0x80;
      }
      if (bVar7 == 2) {
        (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x30))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x30))) + 0x80;
      }
      if (bVar7 == 3) {
        sVar6 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x30))) + -0x80;
LAB_00259d18:
        (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x30))) = sVar6;
      }
    }
    else {
      if (bVar7 == 0) {
        (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x34))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x34))) + 0x80;
      }
      if (bVar7 == 1) {
        (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x34))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x34))) + -0x80;
      }
      if (bVar7 == 2) {
        (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x30))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x30))) + -0x80;
      }
      if (bVar7 == 3) {
        sVar6 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x30))) + 0x80;
        goto LAB_00259d18;
      }
    }
    sVar6 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 6))) + 1;
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 6))) = sVar6;
    if (sVar6 == 0x10) {
      bVar3 = bVar3 & 0xfe;
      (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(2) * sizeof(int8_t))) = bVar3;
    }
  }
  bVar7 = bVar3;
  if ((bVar3 & 2) != 0) {
    bVar4 = (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(3) * sizeof(int8_t)));
    if (bVar4 == 0) {
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x34))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x34))) + -0x400;
    }
    if (bVar4 == 1) {
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x34))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x34))) + 0x400;
    }
    if (bVar4 == 2) {
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x30))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x30))) + 0x400;
    }
    if (bVar4 == 3) {
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x30))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x30))) + -0x400;
      sVar6 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x34)));
    }
    else {
      sVar6 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x34)));
    }
    if ((((sVar6 < -0x47ff) || (0x47ff < sVar6)) || (0x47ff < (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x30))))) ||
       ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x30))) < -0x47ff)) {
      (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(4) * sizeof(int8_t))) = 1;
      bVar7 = bVar3 & 0xfd | 0x10;
      (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(2) * sizeof(int8_t))) = bVar7;
    }
  }
  bVar4 = bVar7 & 8;
  if ((bVar3 & 4) != 0) {
    fVar10 = (*(float*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x24))) + (*(float*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0xc)));
    fVar11 = (*(float*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0xc))) - (*(float*)rtguest::G(0x3330c4));
    (*(float*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x24))) = fVar10;
    (*(float*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0xc))) = fVar11;
    if (fVar10 <= 1.5) {
      sVar6 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 6)));
      (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(0x24) * sizeof(int8_t))) = 0;
      (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(0x25) * sizeof(int8_t))) = 0;
      (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(0x26) * sizeof(int8_t))) = 0xc0;
      (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(0x27) * sizeof(int8_t))) = 0x3f;
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 6))) = sVar6 + 1;
      if ((short)(sVar6 + 1) < 8) {
        (*(float*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0xc))) = fVar11 * -0.5;
      }
      else {
        bVar7 = bVar7 & 0xfb | 0x10;
        (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(2) * sizeof(int8_t))) = bVar7;
      }
      bVar4 = bVar7 & 8;
    }
  }
  if (bVar4 == 0) {
    iVar5 = (uint32_t)bVar7 << 0x18;
  }
  else {
    fVar14 = (*(float*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x14)));
    fVar15 = (*(float*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x1c)));
    fVar13 = (*(float*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 8)));
    fVar10 = fVar15 * (*(float*)rtguest::G(0x3330c8));
    fVar11 = (*(float*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x10)));
    fVar12 = fVar13 + fVar14 * (*(float*)rtguest::G(0x3330c8));
    (*(float*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 8))) = fVar12;
    (*(float*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x10))) = fVar11 + fVar10;
    (*(float*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x20))) = (*(float*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x20))) + fVar13;
    (*(float*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x28))) = (*(float*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x28))) + fVar11;
    if ((fVar14 <= 0.0) || (0.0 <= fVar12)) {
      if ((fVar14 < 0.0) && (0.0 < fVar12)) {
        (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(8) * sizeof(int8_t))) = 0;
        (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(9) * sizeof(int8_t))) = 0;
        (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(10) * sizeof(int8_t))) = 0;
        (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(0xb) * sizeof(int8_t))) = 0;
      }
    }
    else {
      (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(8) * sizeof(int8_t))) = 0;
      (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(9) * sizeof(int8_t))) = 0;
      (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(10) * sizeof(int8_t))) = 0;
      (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(0xb) * sizeof(int8_t))) = 0;
    }
    if ((fVar15 <= 0.0) || (0.0 <= (*(float*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x10))))) {
      if (fVar15 < 0.0) {
        if (0.0 < (*(float*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x10)))) {
          (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(0x10) * sizeof(int8_t))) = 0;
          (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(0x11) * sizeof(int8_t))) = 0;
          (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(0x12) * sizeof(int8_t))) = 0;
          (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(0x13) * sizeof(int8_t))) = 0;
          goto LAB_00259f6c;
        }
        fVar10 = (*(float*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 8)));
      }
      else {
        fVar10 = (*(float*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 8)));
      }
    }
    else {
      (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(0x10) * sizeof(int8_t))) = 0;
      (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(0x11) * sizeof(int8_t))) = 0;
      (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(0x12) * sizeof(int8_t))) = 0;
      (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(0x13) * sizeof(int8_t))) = 0;
LAB_00259f6c:
      fVar10 = (*(float*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 8)));
    }
    iVar5 = (uint32_t)bVar7 << 0x18;
    if ((fVar10 == 0.0) && ((*(float*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x10))) == 0.0)) {
      bVar3 = bVar7 & 0xf7 | 0x10;
      (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(2) * sizeof(int8_t))) = bVar3;
      iVar5 = (uint32_t)bVar3 << 0x18;
    }
  }
  if (iVar5 >> 0x18 == 0x10) {
    RT_CALLRA(0x259fec, guest::vu0SubVector(auStack_50,(*(uint32_t*)rtguest::G((uint32_t)(0x2c3a00u + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(1) * sizeof(int8_t)))) * 4))) + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar1)) * 0x10,0x177ace0));
    fVar10 = rtguest::fbits(RT_CALLRA(0x259ff8, guest::vecDotProductVu0(auStack_50,auStack_50)));
    if (1.5 <= SQRT(fVar10)) {
      RT_CALLRA(0x25a060, guest::vu0CopyVector(pfVar8,(*(uint32_t*)rtguest::G((uint32_t)(0x2c3a00u + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(1) * sizeof(int8_t)))) * 4))) + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar1)) * 0x10));
      (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(2) * sizeof(int8_t))) = 0;
    }
  }
LAB_0025a078:
  RT_CALLRA(0x25a078, guest::TransformRectangle(uStack_60,pbVar1 + 0x30));
  RT_CALLRA(0x25a090, guest::vu0UnitMatrix(0x0177eb10 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar1)) * 0x40));
  RT_CALLRA(0x25a0a8, guest::vu0RotMatrixX((*(uint32_t*)rtguest::G((uint32_t)uStack_60)),0x0177eb10 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar1)) * 0x40,0x0177eb10 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar1)) * 0x40))
  ;
  RT_CALLRA(0x25a0c0, guest::vu0RotMatrixY((*(uint32_t*)rtguest::G((uint32_t)uStack_5c)),0x0177eb10 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar1)) * 0x40,0x0177eb10 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar1)) * 0x40))
  ;
  RT_CALLRA(0x25a0dc, guest::vu0RotMatrixZ((*(uint32_t*)rtguest::G((uint32_t)uStack_58)),0x0177eb10 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar1)) * 0x40,0x0177eb10 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar1)) * 0x40))
  ;
  RT_CALLRA(0x25a0f0, guest::vu0CopyVector(0x0177eb40 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar1)) * 0x10,pbVar1 + 0x20));
  return 0;
}


RT_GUEST_FN(0x00259a08, guest::UpdateNpcTailCarTransform)

}  // namespace guest
