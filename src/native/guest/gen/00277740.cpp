// guest:0x00277740 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t atanf(uint32_t param_1, ...) {
  int iVar1;
uint32_t puVar2;
  float fVar3;
  float fVar4;
uint32_t puVar5;
  float fVar6;
  
  puVar2 = (uint32_t)((uint32_t)param_1 & 0x7fffffff);
  if ((uint32_t)0x507fffff < puVar2) {
    if (0 < (int)param_1) {
      return (uint32_t)0x3fc90fdb;
    }
    return 0xbfc90fdb;
  }
  if (puVar2 < (uint32_t)0x3ee00000) {
    iVar1 = -1;
    puVar5 = param_1;
    if (puVar2 <= 0x30ffffff) {
      if (1.0 < (float)param_1 + 1e+30) {
      }
      fVar6 = (float)param_1 * (float)param_1;
      goto LAB_002778f0;
    }
  }
  else {
    fVar6 = (float)fabsf();
    if (puVar2 < (uint32_t)0x3f980000) {
      if (puVar2 < (uint32_t)0x3f300000) {
        puVar5 = (uint32_t)(((fVar6 + fVar6) - 1.0) / (fVar6 + 2.0));
        iVar1 = 0;
      }
      else {
        puVar5 = (uint32_t)((fVar6 - 1.0) / (fVar6 + 1.0));
        iVar1 = 1;
      }
    }
    else if (puVar2 < (uint32_t)0x401c0000) {
      puVar5 = (uint32_t)((fVar6 - 1.5) / (fVar6 * 1.5 + 1.0));
      iVar1 = 2;
    }
    else {
      iVar1 = 3;
      puVar5 = (uint32_t)(-1.0 / fVar6);
    }
  }
  fVar6 = (float)puVar5 * (float)puVar5;
LAB_002778f0:
  fVar4 = fVar6 * fVar6;
  fVar3 = fVar4 * (fVar4 * (fVar4 * (fVar4 * (fVar4 * -0.03653157 + -0.058335703) + -0.07691876) +
                           -0.111111104) + -0.2);
  fVar6 = fVar6 * (fVar4 * (fVar4 * (fVar4 * (fVar4 * (fVar4 * 0.01628582 + 0.04976878) + 0.06661073
                                             ) + 0.09090887) + 0.14285715) + 0.33333334);
  if (iVar1 < 0) {
    puVar5 = (uint32_t)((float)puVar5 - (float)puVar5 * (fVar6 + fVar3));
  }
  else {
    puVar5 = (uint32_t)
             ((*(float*)rtguest::G((uint32_t)(uint32_t)(0x00331b38 + iVar1 * 4))) -
             (((float)puVar5 * (fVar6 + fVar3) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x00331b48 + iVar1 * 4)))) -
             (float)puVar5));
    if ((int)param_1 < 0) {
      puVar5 = (uint32_t)-(float)puVar5;
    }
  }
}


RT_GUEST_FN(0x00277740, guest::atanf)

}  // namespace guest
