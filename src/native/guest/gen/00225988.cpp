// guest:0x00225988 transliterated from Ghidra pseudocode — DO NOT hand-edit
// weights at 0x177F720 and fog from the clock (vblanks since midnight).
// param_1 carries float bits (all callers pass uint32 loads); fbits() at use sites.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t updateTimeOfDayLight(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(176);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x48, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x58, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x68, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x70, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x78, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0x80, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x88) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x8c) = 0x00000000u;
    rtguest::rtSpillF(rt_fg, 6, rt_sv, 0xa0);
    rtguest::rtSpillF(rt_fg, 5, rt_sv, 0x98);
    rtguest::rtSpillF(rt_fg, 4, rt_sv, 0x90);
    *(uint32_t*)rtguest::G(rt_sv+0x30) = (uint32_t)(int32_t)param_1;
    rtguest::gS[1] = 0x000000000177f5e0ull;  // walk const
  }
  uint8_t uVar1;
  uint8_t bVar2;
  uint8_t uVar3;
  uint8_t uVar4;
  int iVar5;
  int iVar6;
  uint32_t puVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  uint32_t auStack_b0 = rtguest::allocFrame(16);
  uint32_t auStack_a0 = rtguest::allocFrame(16);
  uint32_t auStack_90 = rtguest::allocFrame(16);
  uint32_t iStack_80 = rtguest::allocFrame(4);

  fVar8 = 300.0f;
  float fPar = rtguest::fbits(param_1);
  if (fPar <= 300.0f) {
    fVar8 = fPar;
  }
  if ((uint32_t)(param_3 - 0xc15cU) > 0x1c907) {   // 0x1c907 < param_3 - 0xc15cU
    fVar11 = 0.0f;
    *(uint32_t*)rtguest::G(0x177f73c) = rtguest::f2bits(300.0f);
    *(uint32_t*)rtguest::G(0x177f738) = rtguest::f2bits(1.0f);   // fogFarParam
    *(uint32_t*)rtguest::G(0x177f750) = 0x80080000;              // fogColourRGBA
    *(uint32_t*)rtguest::G(0x177f754) = 0x80000000;              // _DAT_0177f754
    fVar9 = rtguest::fbits(*(uint32_t*)rtguest::G(0x332da8));
    fVar10 = 0.0f;
    goto LAB_00225ed4;
  }
  if ((int32_t)param_3 < 0xe484) {
    fVar10 = rtguest::fdiv((float)(int)(param_3 - 0xc15cU), rtguest::fbits(*(uint32_t*)rtguest::G(0x332dac)));
    fVar11 = 0.0f;
    fVar9 = rtguest::fbits(*(uint32_t*)rtguest::G(0x332db0)) - fVar10;
    uVar1 = (uint8_t)(int)(fVar10 * 255.0f);
    *(uint32_t*)rtguest::G(0x177f754) = (uint32_t)uVar1 << 8 | uVar1;
    if (fVar10 < 0.5f) {
      *(uint32_t*)rtguest::G(0x177f754) =
          ((uint32_t)(uint8_t)(char)(int)(fVar10 * 510.0f) << 16) |
          (*(uint32_t*)rtguest::G(0x177f754) & 0xffff);
    }
    else {
      *(uint32_t*)rtguest::G(0x177f754) =
          0xff0000 | (*(uint32_t*)rtguest::G(0x177f754) & 0xffff);
    }
    if (-2.5f < fVar8) {
      *(uint32_t*)rtguest::G(0x177f738) = rtguest::f2bits(fVar9 * 0.5f + 0.5f);
      *(uint32_t*)rtguest::G(0x177f73c) = rtguest::f2bits((fVar10 * 500.0f + 300.0f) - fVar8);
      if (rtguest::fbits(*(uint32_t*)rtguest::G(0x177f73c)) < 300.0f) {
        *(uint32_t*)rtguest::G(0x177f73c) = rtguest::f2bits(300.0f);
      }
      uVar1 = (uint8_t)(int)(fVar10 * 255.0f);
      *(uint16_t*)rtguest::G(0x177f750) = (uint16_t)((uint32_t)uVar1 << 8 | uVar1);
      if (0.5f <= fVar10) {
        uVar1 = 0xff;
        goto LAB_00225be4;
      }
      *(uint32_t*)rtguest::G(0x177f750) =
          (*(uint32_t*)rtguest::G(0x177f750) & ~0xffffffu) |
          ((uint32_t)(uint8_t)(char)(int)(fVar10 * 510.0f) << 16) |
          (*(uint32_t*)rtguest::G(0x177f750) & 0xffff);   // fog._0_3_ = CONCAT12(b510,(short)fog)
    }
    else {
      *(uint32_t*)rtguest::G(0x177f73c) = rtguest::f2bits(300.0f);
      *(uint32_t*)rtguest::G(0x177f738) = rtguest::f2bits(1.0f);
      bVar2 = (uint8_t)(int)(fVar10 * 127.0f);
      *(uint16_t*)rtguest::G(0x177f750) = (uint16_t)((uint32_t)bVar2 << 8);
      if (fVar10 < 0.5f) {
        uint32_t t = (uint32_t)(uint8_t)(char)(int)(fVar10 * 254.0f) << 16 | (uint32_t)bVar2 << 8;
        *(uint32_t*)rtguest::G(0x177f750) =
            (*(uint32_t*)rtguest::G(0x177f750) & ~0xffffffu) | (t & 0xffffff);
      }
      else {
        uVar1 = 0x7f;
        goto LAB_00225be4;
      }
    }
LAB_00225be4:
    *(uint32_t*)rtguest::G(0x177f750) =   // LAB_00225be4: fogColourRGBA._0_3_ = CONCAT12(uVar1,(short)fog)
        (*(uint32_t*)rtguest::G(0x177f750) & ~0xffffffu) |
        ((uint32_t)uVar1 << 16) | (*(uint32_t*)rtguest::G(0x177f750) & 0xffff);
LAB_00225ecc:
    uVar1 = 0x7f;
  }
  else {
    if ((int32_t)param_3 < 0x255a8) {
      fVar9 = 0.0f;
      *(uint32_t*)rtguest::G(0x177f754) = 0x80ffffff;
      fVar11 = 0.0f;
      fVar10 = rtguest::fbits(*(uint32_t*)rtguest::G(0x332db4));
      if (fVar8 <= -2.5f) {
        *(uint32_t*)rtguest::G(0x177f73c) = rtguest::f2bits(300.0f);
        *(uint32_t*)rtguest::G(0x177f738) = rtguest::f2bits(1.0f);
        *(uint32_t*)rtguest::G(0x177f750) = 0x807f7f00;
      }
      else {
        *(uint32_t*)rtguest::G(0x177f73c) = rtguest::f2bits(800.0f - fVar8);
        *(uint32_t*)rtguest::G(0x177f750) = 0x80ffffff;
        *(uint32_t*)rtguest::G(0x177f738) = rtguest::f2bits(0.5f);
      }
      goto LAB_00225ed4;
    }
    if (0x278cf < (int32_t)param_3) {
      fVar10 = 0.0f;
      fVar9 = rtguest::fdiv((float)(int)(param_3 + -0x278d0), rtguest::fbits(*(uint32_t*)rtguest::G(0x332dc0))) +
              rtguest::fbits(*(uint32_t*)rtguest::G(0x332dc4));
      fVar11 = rtguest::fbits(*(uint32_t*)rtguest::G(0x332dc8)) - fVar9;
      uVar1 = (uint8_t)(int)(fVar11 * 255.0f);
      uVar3 = (uint8_t)(int)(fVar11 * 161.0f);
      *(uint32_t*)rtguest::G(0x177f754) = (uint32_t)uVar3 << 8 | uVar1;
      uVar4 = (uint8_t)(int)(fVar11 * 127.0f);
      *(uint32_t*)rtguest::G(0x177f754) =
          ((uint32_t)uVar4 << 16) | (*(uint32_t*)rtguest::G(0x177f754) & 0xffff);
      if (-2.5f < fVar8) {
        *(uint32_t*)rtguest::G(0x177f738) = rtguest::f2bits(fVar9 * 0.5f + 0.5f);
        *(uint32_t*)rtguest::G(0x177f73c) = rtguest::f2bits((fVar11 * 500.0f + 300.0f) - fVar8);
        if (rtguest::fbits(*(uint32_t*)rtguest::G(0x177f73c)) < 300.0f) {
          *(uint32_t*)rtguest::G(0x177f73c) = rtguest::f2bits(300.0f);
        }
        *(uint32_t*)rtguest::G(0x177f750) =
            (*(uint32_t*)rtguest::G(0x177f750) & ~0xffffffu) |
            (uint32_t)uVar4 << 16 | (uint32_t)uVar3 << 8 | uVar1;
      }
      else {
        *(uint32_t*)rtguest::G(0x177f73c) = rtguest::f2bits(300.0f);
        *(uint32_t*)rtguest::G(0x177f738) = rtguest::f2bits(1.0f);
        *(uint32_t*)rtguest::G(0x177f750) =
            (*(uint32_t*)rtguest::G(0x177f750) & ~0xffffffu) |
            (uint32_t)(uint8_t)(char)(int)(fVar11 * 63.0f) << 16 |
            (uint32_t)(uint8_t)(char)(int)(fVar11 * 80.0f) << 8 |
            (uint8_t)(int)(fVar11 * 0.0f);
      }
      goto LAB_00225ecc;
    }
    fVar11 = rtguest::fdiv((float)(int)(param_3 + -0x255a8), rtguest::fbits(*(uint32_t*)rtguest::G(0x332db8)));
    fVar9 = 0.0f;
    fVar10 = rtguest::fbits(*(uint32_t*)rtguest::G(0x332dbc)) - fVar11;
    uVar1 = (uint8_t)(int)(255.0f - fVar11 * 94.0f);
    *(uint32_t*)rtguest::G(0x177f754) = (uint32_t)uVar1 << 8 | 0xff;
    uVar3 = (uint8_t)(int)(255.0f - fVar11 * 128.0f);
    *(uint32_t*)rtguest::G(0x177f754) =
        ((uint32_t)uVar3 << 16) | (*(uint32_t*)rtguest::G(0x177f754) & 0xffff);
    if (-2.5f < fVar8) {
      *(uint32_t*)rtguest::G(0x177f73c) = rtguest::f2bits(800.0f - fVar8);
      *(uint32_t*)rtguest::G(0x177f738) = rtguest::f2bits(0.5f);
      *(uint16_t*)rtguest::G(0x177f750) = (uint16_t)((uint32_t)uVar1 << 8 | 0xff);
      *(uint32_t*)rtguest::G(0x177f750) =
          (*(uint32_t*)rtguest::G(0x177f750) & ~0xffffffu) |
          ((uint32_t)uVar3 << 16) | (*(uint32_t*)rtguest::G(0x177f750) & 0xffff);
    }
    else {
      *(uint32_t*)rtguest::G(0x177f73c) = rtguest::f2bits(300.0f);
      *(uint32_t*)rtguest::G(0x177f738) = rtguest::f2bits(1.0f);
      *(uint32_t*)rtguest::G(0x177f750) =
          (*(uint32_t*)rtguest::G(0x177f750) & ~0xffffffu) |
          (uint32_t)(uint8_t)(char)(int)(127.0f - fVar11 * 64.0f) << 16 |
          (uint32_t)(uint8_t)(char)(int)(127.0f - fVar11 * 47.0f) << 8;
    }
    uVar1 = 0x80;
  }
  *(uint8_t*)rtguest::G(0x177f753) = uVar1;   // fogColourRGBA = CONCAT13(uVar1,(int3)fog)
LAB_00225ed4:
  iVar6 = 0;
  puVar7 = 0x177f620;  // lightMatrixBlend
  *(uint32_t*)rtguest::G(0x177f720) = rtguest::f2bits(fVar10);  // sceneParams
  *(uint32_t*)rtguest::G(0x177f724) = rtguest::f2bits(fVar11);
  *(uint32_t*)rtguest::G(0x177f728) = rtguest::f2bits(fVar9);
  *(uint32_t*)rtguest::G((uint32_t)iStack_80) = param_2;
  do {
    iVar5 = iVar6 * 0x10;
    iVar6 = iVar6 + 1;
    RT_CALLRA(0x225f18, guest::vu0ScaleVector(fVar10,auStack_b0,0x29d500 + iVar5));   // todLightDay
    RT_CALLRA(0x225f28, guest::vu0ScaleVector(fVar11,auStack_a0,0x29d540 + iVar5));   // todLightDusk
    RT_CALLRA(0x225f38, guest::vu0ScaleVector(fVar9,auStack_90,0x29d580 + iVar5));    // todLightNight
    RT_CALLRA(0x225f48, guest::vu0AddVector(auStack_b0,auStack_b0,auStack_a0));
    RT_CALLRA(0x225f58, guest::vu0AddVector(puVar7,auStack_b0,auStack_90));
    puVar7 = puVar7 + 0x10;
  } while (iVar6 < 4);
  *(uint32_t*)rtguest::G(0x177f65c) = 0x43000000;
  iVar6 = 0;
  puVar7 = 0x177f660;  // lightMatrixBlend2
  do {
    iVar5 = iVar6 * 0x10;
    iVar6 = iVar6 + 1;
    RT_CALLRA(0x225fb0, guest::vu0ScaleVector(fVar10,auStack_b0,0x29d5c0 + iVar5));   // todLight2Day
    RT_CALLRA(0x225fc0, guest::vu0ScaleVector(fVar11,auStack_a0,0x29d600 + iVar5));   // todLight2Dusk
    RT_CALLRA(0x225fd0, guest::vu0ScaleVector(fVar9,auStack_90,0x29d640 + iVar5));    // todLight2Night
    RT_CALLRA(0x225fe0, guest::vu0AddVector(auStack_b0,auStack_b0,auStack_a0));
    RT_CALLRA(0x225ff0, guest::vu0AddVector(puVar7,auStack_b0,auStack_90));
    puVar7 = puVar7 + 0x10;
  } while (iVar6 < 4);
  *(uint32_t*)rtguest::G(0x177f69c) = 0x43000000;
  // disasm 0x226010 lw $v1,0x30($sp) + 0x22604c sw $v0,0x50($v1): the slot's
  // VALUE is the task pointer (param_2 stored above) — dereference it. The
  // transliterator used the slot address + 0x50, which wrote fog into the
  // stack frame instead of the light task.
  *(uint32_t*)rtguest::G(*(uint32_t*)rtguest::G((uint32_t)iStack_80) + 0x50) = *(uint32_t*)rtguest::G(0x177f750);
  return 0;
}


RT_GUEST_FN(0x00225988, guest::updateTimeOfDayLight)

}  // namespace guest
