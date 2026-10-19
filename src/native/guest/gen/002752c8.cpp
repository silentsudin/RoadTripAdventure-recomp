// guest:0x002752c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __ieee754_rem_pio2f(uint32_t param_1, uint32_t param_2, ...) {
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
  (*(float*)rtguest::G((uint32_t)afStack_3c + (uint32_t)(1) * 4)) = ABS(param_1);
  pfVar4 = (uint32_t)param_2;
  if ((uint32_t)(*(float*)rtguest::G((uint32_t)afStack_3c + (uint32_t)(1) * 4)) < 0x3f490fd9) {
    (*(float*)rtguest::G((uint32_t)pfVar4)) = param_1;
    (*(float*)rtguest::G((uint32_t)pfVar4 + (uint32_t)(1) * sizeof(float))) = 0.0;
    return 0;
  }
  if ((uint32_t)(*(float*)rtguest::G((uint32_t)afStack_3c + (uint32_t)(1) * 4)) < 0x4016cbe4) {
    if (0 < (int)param_1) {
      fVar8 = param_1 - 1.5707855;
      if (((uint32_t)param_1 & 0x7ffffff0) == 0x3fc90fd0) {
        fVar6 = 6.0771e-11;
        fVar8 = fVar8 - 1.0804273e-05;
        fVar5 = fVar8 - 6.0771e-11;
      }
      else {
        fVar6 = 1.0804334e-05;
        fVar5 = fVar8 - 1.0804334e-05;
      }
      (*(float*)rtguest::G((uint32_t)pfVar4)) = fVar5;
      (*(float*)rtguest::G((uint32_t)pfVar4 + (uint32_t)(1) * sizeof(float))) = (fVar8 - fVar5) - fVar6;
      return 1;
    }
    fVar8 = param_1 + 1.5707855;
    if (((uint32_t)param_1 & 0x7ffffff0) == 0x3fc90fd0) {
      fVar6 = 6.0771e-11;
      fVar8 = fVar8 + 1.0804273e-05;
      fVar5 = fVar8 + 6.0771e-11;
    }
    else {
      fVar6 = 1.0804334e-05;
      fVar5 = fVar8 + 1.0804334e-05;
    }
    (*(float*)rtguest::G((uint32_t)pfVar4)) = fVar5;
    (*(float*)rtguest::G((uint32_t)pfVar4 + (uint32_t)(1) * sizeof(float))) = (fVar8 - fVar5) + fVar6;
    return -1;
  }
  iVar1 = (int)(*(float*)rtguest::G((uint32_t)afStack_3c + (uint32_t)(1) * 4)) >> 0x17;
  if (0x43490f80 < (uint32_t)(*(float*)rtguest::G((uint32_t)afStack_3c + (uint32_t)(1) * 4))) {
    (*(float*)rtguest::G((uint32_t)afStack_3c + (uint32_t)(1) * 4)) = (float)((int)(*(float*)rtguest::G((uint32_t)afStack_3c + (uint32_t)(1) * 4)) + (iVar1 + -0x86) * -0x800000);
    iVar3 = 1;
    do {
      iVar3 = iVar3 + -1;
      (*(float*)rtguest::G((uint32_t)pfVar2)) = (float)(int)(*(float*)rtguest::G((uint32_t)afStack_3c + (uint32_t)(1) * 4));
      pfVar2 = pfVar2 + 1;
      (*(float*)rtguest::G((uint32_t)afStack_3c + (uint32_t)(1) * 4)) = ((*(float*)rtguest::G((uint32_t)afStack_3c + (uint32_t)(1) * 4)) - (float)(int)(*(float*)rtguest::G((uint32_t)afStack_3c + (uint32_t)(1) * 4))) * 256.0;
    } while (-1 < iVar3);
    iVar3 = 3;
    if ((*(float*)rtguest::G((uint32_t)afStack_3c + (uint32_t)(1) * 4)) == 0.0) {
      pfVar2 = afStack_3c + 1;
      do {
        pfVar2 = pfVar2 + -1;
        iVar3 = iVar3 + -1;
      } while ((*(float*)rtguest::G((uint32_t)pfVar2)) == 0.0);
    }
    iVar3 = __kernel_rem_pio2f(fStack_40,param_2,iVar1 + -0x86,iVar3,2,0x331560);
    if (-1 < (int)param_1) {
      return iVar3;
    }
    fVar5 = (*(float*)rtguest::G((uint32_t)pfVar4));
    fVar6 = (*(float*)rtguest::G((uint32_t)pfVar4 + (uint32_t)(1) * sizeof(float)));
    goto LAB_0027566c;
  }
  fVar8 = (float)fabsf();
  iVar3 = (int)(fVar8 * 0.6366198 + 0.5);
  fVar5 = (float)iVar3;
  fVar6 = fVar5 * 1.0804334e-05;
  fVar8 = fVar8 - fVar5 * 1.5707855;
  if ((iVar3 < 0x20) && (((uint32_t)param_1 & 0x7fffff00) != (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x00331874 + iVar3 * 4))))) {
    (*(float*)rtguest::G((uint32_t)pfVar4)) = fVar8 - fVar6;
LAB_00275584:
    fVar5 = (*(float*)rtguest::G((uint32_t)pfVar4));
    fVar7 = fVar8;
  }
  else {
    (*(float*)rtguest::G((uint32_t)pfVar4)) = fVar8 - fVar6;
    if ((int)(iVar1 - ((uint32_t)(fVar8 - fVar6) >> 0x17 & 0xff)) < 9) {
      fVar5 = (*(float*)rtguest::G((uint32_t)pfVar4));
      fVar7 = fVar8;
    }
    else {
      fVar7 = fVar8 - fVar5 * 1.0804273e-05;
      fVar6 = fVar5 * 6.0771e-11 - ((fVar8 - fVar7) - fVar5 * 1.0804273e-05);
      (*(float*)rtguest::G((uint32_t)pfVar4)) = fVar7 - fVar6;
      if (0x19 < (int)(iVar1 - ((uint32_t)(fVar7 - fVar6) >> 0x17 & 0xff))) {
        fVar8 = fVar7 - fVar5 * 6.0770944e-11;
        fVar6 = fVar5 * 6.123234e-17 - ((fVar7 - fVar8) - fVar5 * 6.0770944e-11);
        (*(float*)rtguest::G((uint32_t)pfVar4)) = fVar8 - fVar6;
        goto LAB_00275584;
      }
      fVar5 = (*(float*)rtguest::G((uint32_t)pfVar4));
    }
  }
  fVar6 = (fVar7 - fVar5) - fVar6;
  (*(float*)rtguest::G((uint32_t)pfVar4 + (uint32_t)(1) * sizeof(float))) = fVar6;
  if (-1 < (int)param_1) {
    return iVar3;
  }
LAB_0027566c:
  (*(float*)rtguest::G((uint32_t)pfVar4)) = -fVar5;
  (*(float*)rtguest::G((uint32_t)pfVar4 + (uint32_t)(1) * sizeof(float))) = -fVar6;
  return -iVar3;
}


RT_GUEST_FN(0x002752c8, guest::__ieee754_rem_pio2f)

}  // namespace guest
