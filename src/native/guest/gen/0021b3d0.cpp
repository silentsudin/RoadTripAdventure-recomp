// guest:0x0021b3d0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t ResolveGroundContact(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
  rtguest::FrameGuard rt_fg(272);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0xa8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0xb0, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0xb8, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0xc0, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0xc8, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0xd0, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0xd8, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0xe0, 8);
    rtguest::rtSpillF(rt_fg, 4, rt_sv, 0xf0);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0xa0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0xe8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xec) = 0x00000000u;
    rtguest::rtSpillF(rt_fg, 6, rt_sv, 0x100);
    rtguest::rtSpillF(rt_fg, 5, rt_sv, 0xf8);
    rtguest::setS32(2, (uint32_t)param_2);  // walk: = param_2
    rtguest::gS[3] = 0x0000000000000000ull;  // walk const
    rtguest::setS32(6, (uint32_t)param_1);  // walk: = param_1
    rtguest::gS[7] = 0x0000000000000000ull;  // walk const
    rtguest::gS[8] = rt_sv + 0x20ull;  // walk: sp+0x20
  }
uint32_t auStack_110 = rtguest::allocFrame(32);
uint32_t aiStack_f0 = rtguest::allocFrame(32);
    uint32_t fStack_d0 = rtguest::allocFrame(4);
    uint32_t fStack_cc = rtguest::allocFrame(4);
    uint32_t fStack_c8 = rtguest::allocFrame(4);
    uint32_t uStack_c4 = rtguest::allocFrame(4);
    uint32_t fStack_c0 = rtguest::allocFrame(4);
    uint32_t fStack_bc = rtguest::allocFrame(4);
    uint32_t fStack_b8 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t fStack_b0 = rtguest::allocFrame(4);
    uint32_t fStack_ac = rtguest::allocFrame(4);
    uint32_t fStack_a8 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
uint32_t auStack_a0 = rtguest::allocFrame(4);
    uint32_t fStack_9c = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
uint32_t auStack_90 = rtguest::allocFrame(4);
    uint32_t uStack_8c = rtguest::allocFrame(4);
    uint32_t iStack_88 = rtguest::allocFrame(4);
    uint32_t iStack_84 = rtguest::allocFrame(4);
    uint32_t uStack_80 = rtguest::allocFrame(4);
    uint32_t iStack_7c = rtguest::allocFrame(4);
    uint32_t iStack_78 = rtguest::allocFrame(4);
  ushort uVar1;
  uint uVar2;
  long lVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
uint32_t pfVar7;
  int iVar8;
  uint uVar9;
uint32_t puVar10;
uint32_t pfVar11;
  uint uVar12;
  float fVar13;
  float fVar14;
  
  uVar9 = 0;
  iVar8 = (int)param_2;
  pfVar11 = (uint32_t)(iVar8 + 0x110);
  uVar12 = 0;
  uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0xa8)));
  iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0xa0)));
  (*(float*)rtguest::G((uint32_t)fStack_cc)) = rtguest::fdiv((float)(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0xa4))), (*(float*)rtguest::G(0x332bfc)));
  uVar5 = uVar2;
  if ((int)uVar2 < 0) {
    uVar5 = uVar2 + 0x1ffffff;
  }
  iVar4 = iVar6;
  if (iVar6 < 0) {
    iVar4 = iVar6 + 0xffffff;
  }
  (*(float*)rtguest::G((uint32_t)fStack_c8)) = rtguest::fdiv((float)(uVar2 & 0x1ffffff), (*(float*)rtguest::G(0x332bfc)));
  uVar5 = (int)uVar5 >> 0x19 & 1;
  (*(int32_t*)rtguest::G((uint32_t)iStack_78)) = ((iVar4 >> 0x18) - uVar5 & 2) + uVar5;
  (*(int32_t*)rtguest::G((uint32_t)iStack_7c)) = iVar8 + 0x19c;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c4)) = 0x3f800000;
  (*(float*)rtguest::G((uint32_t)fStack_d0)) = rtguest::fdiv((float)(iVar6 + ((int)(uVar2 & 0x2000000) >> 1) & 0x1ffffff), (*(float*)rtguest::G(0x332bfc)));
  puVar10 = 0x0029ca80;
  pfVar7 = pfVar11;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_8c)) = param_3;
  (*(int32_t*)rtguest::G((uint32_t)iStack_88)) = param_4;
  (*(int32_t*)rtguest::G((uint32_t)iStack_84)) = param_5;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_80)) = param_6;
  do {
    RT_CALLRA(0x21b528, guest::vu0ApplyMatrix(pfVar7,param_2,puVar10));
    RT_CALLRA(0x21b538, guest::vu0AddVector(pfVar7,pfVar7,fStack_d0));
    iVar6 = uVar9 * 4;
    (*(int32_t*)rtguest::G((uint32_t)aiStack_f0 + (uint32_t)(uVar9) * 4)) = (int)((*(float*)rtguest::G((uint32_t)pfVar7 + (uint32_t)(1) * sizeof(float))) * 32768.0);
    lVar3 = RT_CALLRA(0x0021b570, rtCall((*(uint32_t*)rtguest::G(0x335710)), pfVar7, auStack_110 + iVar6, auStack_90, (*(int32_t*)rtguest::G((uint32_t)iStack_78)), param_1));
    uVar2 = uVar9 & 0x1f;
    uVar9 = uVar9 + 1;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + (*(int32_t*)rtguest::G((uint32_t)iStack_7c))))) = (int)lVar3;
    if (lVar3 == -1) {
      uVar12 = uVar12 | 1 << uVar2;
    }
    puVar10 = puVar10 + 0x10;
    pfVar7 = pfVar7 + 0x10;  // T* element step
  } while ((int)uVar9 < 7);
  iVar6 = (*(int32_t*)rtguest::G((uint32_t)iStack_84));
  if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x184))) + 8))) & 8) == 0) {
    iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_88)) + 0xc))) + (*(int32_t*)rtguest::G((uint32_t)iStack_84));
  }
  uVar2 = RT_CALLRA(0x21b5e0, guest::UpdateWheelSuspension(param_1,param_2,aiStack_f0,auStack_110,(*(uint32_t*)rtguest::G((uint32_t)uStack_8c)),pfVar11,iVar6));
  uVar12 = uVar12 | uVar2;
  iVar6 = (((*(int32_t*)rtguest::G((uint32_t)aiStack_f0 + (uint32_t)(0) * 4)) * 0x21 + (((*(int32_t*)rtguest::G((uint32_t)aiStack_f0 + (uint32_t)(1) * 4)) + (*(int32_t*)rtguest::G((uint32_t)aiStack_f0 + (uint32_t)(2) * 4))) / 2) * 0x22) / 0x43 << 4) / 0x19
  ;
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0xa4))) = iVar6;
  iVar4 = (int)param_1;
  if ((((uVar12 & 0x100) == 0) && ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xb))) != '\0')) &&
     (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x198))) & 3) != 0)) {
    uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x198))) & 1) * 2 + iVar4 + 0x2c)));
    if ((uVar1 & 0x1000) == 0) {
      iVar6 = iVar6 + 0x51eb;
      if ((uVar1 & 0x4000) == 0) goto LAB_0021b6a0;
    }
    else {
      iVar6 = iVar6 + -0x1000;
    }
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0xa4))) = iVar6;
  }
LAB_0021b6a0:
  // fn/0021b3d0.c:114: DAT_00332c00 is a float (lwc1 $f0,-0x7ef0($gp) @0x21b6a0, =1.35f); was read as u32 bits and value-converted.
  fVar13 = (*(float*)rtguest::G(0x332c00));
  if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x177ea50)) + 8))) & 0x400) == 0) {
    fVar13 = 0.5;
  }
  if ((*(float*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x11c))) - fVar13 <= (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x94)))) {
    if ((*(float*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x11c))) <= (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x94)))) {
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x213))) == '\0') {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x213))) = 0;
      }
      else {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1f8))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1f8))) | 0x40;
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x213))) = 0;
      }
      iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x184)));
    }
    else {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1f8))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1f8))) | 0x40;
      if ((((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x213))) == '\0') && (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x198))) & 3) != 0)) &&
         (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x28))) & 0x48) == 0)) {
        RT_CALLRA(0x21b81c, guest::PlayUiSound(0x28));
      }
      iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x184)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x213))) = 0xff;
    }
    goto LAB_0021b85c;
  }
  iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x184)));
  if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) & 0x100) == 0) {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x213))) < '\x01') {
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c0))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c0))) / 2;
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c4))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c4))) / 2;
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c8))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c8))) / 2;
LAB_0021b7ac:
      uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x19c)));
    }
    else {
      uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x19c)));
    }
  }
  else {
    if ((uVar12 & 0x100) == 0) {
      iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_88)) + 8)));
      if (iVar4 < 0x2001) {
        if (iVar4 < 0) {
          iVar4 = iVar4 + 7;
        }
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0xa4))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0xa4))) + (iVar4 >> 3);
      }
      else {
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0xa4))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0xa4))) + 0x400;
        if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c0))) != 0) {
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c0))) = 1;
        }
        if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c4))) != 0) {
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c4))) = 1;
        }
        if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c8))) != 0) {
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c8))) = 1;
        }
      }
      goto LAB_0021b7ac;
    }
    uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x19c)));
  }
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x213))) = 1;
  uVar2 = uVar2 & 0x3000 | 0x100651;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1a0))) = uVar2;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x19c))) = uVar2;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1a4))) = uVar2;
LAB_0021b85c:
  if (((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1dc))) == 0 && (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1e0))) == 0) && (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1e4))) == 0
     ) {
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 500)));
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 500))) = iVar4 + 1;
    if (0x40 < iVar4) {
      iVar4 = ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c0))) + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c4))) + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c8)))) / 3;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1f8))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1f8))) | 1;
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c0))) = iVar4;
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c8))) = iVar4;
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c4))) = iVar4;
    }
  }
  else {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 500))) = 0;
  }
  // fn/0021b3d0.c:199: DAT_00332c04 is a float (lwc1 $f22,-0x7eec($gp) @0x21b94c, =pi); was read as u32 bits -> yaw ~8e7 rad.
  fVar13 = (*(float*)rtguest::G(0x332c04));
  uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1f8))) & 1;
  if ((uVar12 & 0x78) != 0) {
    uVar2 = 1;
  }
  if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) & 8) != 0) {
    if (((*(uint32_t*)rtguest::G((uint32_t)uStack_80)) & 0x8000) != 0) {
      uVar2 = 0x40;
    }
    if (((*(uint32_t*)rtguest::G((uint32_t)uStack_80)) & 0x2000) != 0) {
      uVar2 = 0x80;
    }
  }
  (*(float*)rtguest::G((uint32_t)fStack_c0)) = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x130))) - (*(float*)rtguest::G((uint32_t)pfVar11));
  (*(float*)rtguest::G((uint32_t)fStack_bc)) = (float)((*(int32_t*)rtguest::G((uint32_t)aiStack_f0 + (uint32_t)(2) * 4)) - (*(int32_t*)rtguest::G((uint32_t)aiStack_f0 + (uint32_t)(0) * 4))) * 3.0517578e-05f;
  (*(float*)rtguest::G((uint32_t)fStack_ac)) = (float)((*(int32_t*)rtguest::G((uint32_t)aiStack_f0 + (uint32_t)(1) * 4)) - (*(int32_t*)rtguest::G((uint32_t)aiStack_f0 + (uint32_t)(2) * 4))) * 3.0517578e-05f;
  (*(float*)rtguest::G((uint32_t)fStack_b8)) = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x138))) - (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x118)));
  fVar14 = (float)(int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1d4))) * (*(float*)rtguest::G(0x332c04)) * 3.0517578e-05f;  // fn/0021b3d0.c:216: float DAT_00332c04 (see :199)
  (*(float*)rtguest::G((uint32_t)fStack_b0)) = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x120))) - (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x130)));
  (*(float*)rtguest::G((uint32_t)fStack_a8)) = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x128))) - (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x138)));
  RT_CALLRA(0x21b9b0, guest::vecCrossProductVu0(auStack_a0,fStack_c0,fStack_b0));
  if ((*(float*)rtguest::G((uint32_t)fStack_9c)) < 0.0) {
    (*(float*)rtguest::G((uint32_t)fStack_9c)) = -(*(float*)rtguest::G((uint32_t)fStack_9c));
  }
  RT_CALLRA(0x21b9dc, guest::vecNormalizeVu0(auStack_a0,auStack_a0));
  if (uVar2 != 0) {
    RT_CALLRA(0x21b9f0, guest::SteerHeadingVector(rtguest::f2bits(fVar14),auStack_a0,uVar2));
  }
  RT_CALLRA(0x21b9fc, guest::BuildYawTransform(param_2,auStack_a0));
  RT_CALLRA(0x21ba1c, guest::ApplyMatrixRotation(rtguest::f2bits((float)(int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1d4))) * fVar13 * 3.0517578e-05f),param_2));
  RT_CALLRA(0x21ba28, guest::mat4InvertAffine(iVar8 + 0xb0,param_2));
  return (int)uVar12 >> 3;
}


RT_GUEST_FN(0x0021b3d0, guest::ResolveGroundContact)

}  // namespace guest
