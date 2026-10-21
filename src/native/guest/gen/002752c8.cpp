// guest:0x002752c8 translated from Ghidra pseudocode and ELF disassembly
// param_1 carries x's f12 float bits (mfc1 $18,$f1 at 0x2752e0); param_2 is the
// q0 pointer (a0 set by caller's delay slot; lo lands at ptr+4).
// (uint)float-slot tests are bit reinterprets (mfc1): f2bits(); the exponent
// adjust at 0x2753xx is addu on bits. Guards' (int)x are cvt.w.s truncations.
// Float literals carry f: the guest computed in single precision.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __ieee754_rem_pio2f(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x28, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x30, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x38) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x3c) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
  }
    uint32_t fStack_40 = rtguest::allocFrame(4);
  int iVar1;
uint32_t pfVar2;
  int iVar3;
uint32_t pfVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
uint32_t afStack_3c = rtguest::allocFrame(28);

  pfVar2 = fStack_40;
  // afStack_3c[1] = ABS(x): mfc1 bits + and 0x7fffffff, kept in a float slot
  (*(float*)rtguest::G((uint32_t)afStack_3c + (uint32_t)(1) * 4)) = rtguest::fbits(param_1 & 0x7fffffffu);
  pfVar4 = (uint32_t)param_2;
  if (rtguest::f2bits(*(float*)rtguest::G((uint32_t)afStack_3c + (uint32_t)(1) * 4)) < 0x3f490fd9u) {
    (*(uint32_t*)rtguest::G((uint32_t)pfVar4)) = param_1;  // fq[0] = x (bit copy)
    (*(float*)rtguest::G((uint32_t)pfVar4 + (uint32_t)(1) * sizeof(float))) = 0.0;
    return 0;
  }
  if (rtguest::f2bits(*(float*)rtguest::G((uint32_t)afStack_3c + (uint32_t)(1) * 4)) < 0x4016cbe4u) {
    float x = rtguest::fbits(param_1);
    if (0 < (int32_t)param_1) {  // 0x27532c: blez on x's raw bits
      fVar8 = x - 1.5707855f;
      if ((param_1 & 0x7ffffff0u) == 0x3fc90fd0u) {
        fVar6 = 6.0771e-11f;
        fVar8 = fVar8 - 1.0804273e-05f;
        fVar5 = fVar8 - 6.0771e-11f;
      }
      else {
        fVar6 = 1.0804334e-05f;
        fVar5 = fVar8 - 1.0804334e-05f;
      }
      (*(float*)rtguest::G((uint32_t)pfVar4)) = fVar5;
      (*(float*)rtguest::G((uint32_t)pfVar4 + (uint32_t)(1) * sizeof(float))) = (fVar8 - fVar5) - fVar6;
      return 1;
    }
    fVar8 = x + 1.5707855f;
    if ((param_1 & 0x7ffffff0u) == 0x3fc90fd0u) {
      fVar6 = 6.0771e-11f;
      fVar8 = fVar8 + 1.0804273e-05f;
      fVar5 = fVar8 + 6.0771e-11f;
    }
    else {
      fVar6 = 1.0804334e-05f;
      fVar5 = fVar8 + 1.0804334e-05f;
    }
    (*(float*)rtguest::G((uint32_t)pfVar4)) = fVar5;
    (*(float*)rtguest::G((uint32_t)pfVar4 + (uint32_t)(1) * sizeof(float))) = (fVar8 - fVar5) + fVar6;
    return (uint32_t)-1;
  }
  iVar1 = (int32_t)(rtguest::f2bits(*(float*)rtguest::G((uint32_t)afStack_3c + (uint32_t)(1) * 4)) >> 0x17);
  if (0x43490f80u < rtguest::f2bits(*(float*)rtguest::G((uint32_t)afStack_3c + (uint32_t)(1) * 4))) {
    // addu exponent adjust on the slot's bits
    (*(float*)rtguest::G((uint32_t)afStack_3c + (uint32_t)(1) * 4)) = rtguest::fbits(rtguest::f2bits(*(float*)rtguest::G((uint32_t)afStack_3c + (uint32_t)(1) * 4)) + (uint32_t)((iVar1 + -0x86) * -0x800000));
    iVar3 = 1;
    do {
      iVar3 = iVar3 + -1;
      (*(float*)rtguest::G((uint32_t)pfVar2)) = (float)(int32_t)*(float*)rtguest::G((uint32_t)afStack_3c + (uint32_t)(1) * 4);  // cvt.s.w value digits
      pfVar2 = pfVar2 + 0x4;  // T* element step
      (*(float*)rtguest::G((uint32_t)afStack_3c + (uint32_t)(1) * 4)) = ((*(float*)rtguest::G((uint32_t)afStack_3c + (uint32_t)(1) * 4)) - (float)(int32_t)*(float*)rtguest::G((uint32_t)afStack_3c + (uint32_t)(1) * 4)) * 256.0f;
    } while (-1 < iVar3);
    iVar3 = 3;
    if ((*(float*)rtguest::G((uint32_t)afStack_3c + (uint32_t)(1) * 4)) == 0.0) {
      pfVar2 = afStack_3c + 4;  // float* +1 = +4 bytes (0x2753xx la)
      do {
        pfVar2 = pfVar2 + -4;  // float* -1 = -4 bytes
        iVar3 = iVar3 + -1;
      } while ((*(float*)rtguest::G((uint32_t)pfVar2)) == 0.0);
    }
    iVar3 = RT_CALLRA(0x27564c, guest::__kernel_rem_pio2f(fStack_40,param_2,iVar1 + -0x86,iVar3,2,0x331560));
    if (-1 < (int32_t)param_1) {  // blez on bits
      return iVar3;
    }
    fVar5 = (*(float*)rtguest::G((uint32_t)pfVar4));
    fVar6 = (*(float*)rtguest::G((uint32_t)pfVar4 + (uint32_t)(1) * sizeof(float)));
    goto LAB_0027566c;
  }
  fVar8 = rtguest::fbits(param_1 & 0x7fffffffu);  // (float)fabsf() mangled arg: mfc1+and
  iVar3 = (int32_t)(fVar8 * 0.6366198f + 0.5f);   // cvt.w.s truncation
  fVar5 = (float)iVar3;
  fVar6 = fVar5 * 1.0804334e-05f;
  fVar8 = fVar8 - fVar5 * 1.5707855f;
  if ((iVar3 < 0x20) && ((param_1 & 0x7fffff00u) != (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x00331874 + iVar3 * 4))))) {
    (*(float*)rtguest::G((uint32_t)pfVar4)) = fVar8 - fVar6;
LAB_00275584:
    fVar5 = (*(float*)rtguest::G((uint32_t)pfVar4));
    fVar7 = fVar8;
  }
  else {
    (*(float*)rtguest::G((uint32_t)pfVar4)) = fVar8 - fVar6;
    if ((int32_t)(iVar1 - ((int32_t)(rtguest::f2bits(fVar8 - fVar6) >> 0x17) & 0xff)) < 9) {
      fVar5 = (*(float*)rtguest::G((uint32_t)pfVar4));
      fVar7 = fVar8;
    }
    else {
      fVar7 = fVar8 - fVar5 * 1.0804334e-05f;
      fVar6 = fVar5 * 6.0771e-11f - ((fVar8 - fVar7) - fVar5 * 1.0804334e-05f);
      (*(float*)rtguest::G((uint32_t)pfVar4)) = fVar7 - fVar6;
      if (0x19 < (int32_t)(iVar1 - ((int32_t)(rtguest::f2bits(fVar7 - fVar6) >> 0x17) & 0xff))) {
        fVar8 = fVar7 - fVar5 * 6.0770944e-11f;
        fVar6 = fVar5 * 6.123234e-17f - ((fVar7 - fVar8) - fVar5 * 6.0770944e-11f);
        (*(float*)rtguest::G((uint32_t)pfVar4)) = fVar8 - fVar6;
        goto LAB_00275584;
      }
      fVar5 = (*(float*)rtguest::G((uint32_t)pfVar4));
    }
  }
  fVar6 = (fVar7 - fVar5) - fVar6;
  (*(float*)rtguest::G((uint32_t)pfVar4 + (uint32_t)(1) * sizeof(float))) = fVar6;
  if (-1 < (int32_t)param_1) {  // blez on bits
    return iVar3;
  }
LAB_0027566c:
  (*(float*)rtguest::G((uint32_t)pfVar4)) = -fVar5;
  (*(float*)rtguest::G((uint32_t)pfVar4 + (uint32_t)(1) * sizeof(float))) = -fVar6;
  return (uint32_t)-iVar3;
}


RT_GUEST_FN(0x002752c8, guest::__ieee754_rem_pio2f)

}  // namespace guest
