// guest:0x0026f798 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceGsSetDefDispEnv(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
  int iVar1;
  short sVar2;
uint32_t psVar3;
  undefined8 uVar4;
  long lVar5;
  ulong uVar6;
  ulong uVar7;
  ulong uVar8;
  ulong uVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  int iVar13;
  
  iVar10 = (int)param_3;
  iVar12 = (int)param_4;
  iVar13 = (int)param_5;
  iVar11 = (int)param_6;
  psVar3 = (uint32_t)sceGsGetEnv();
  (*(uint64_t*)rtguest::G((uint32_t)param_1)) = 0x66;
  sVar2 = (*(int16_t*)rtguest::G((uint32_t)psVar3));
  if ((*(int16_t*)rtguest::G((uint32_t)psVar3)) == 0) {
    uVar4 = 2;
  }
  else {
    uVar4 = 3;
    if ((*(int16_t*)rtguest::G((uint32_t)psVar3 + (uint32_t)(2) * sizeof(int16_t))) == 0) {
      uVar4 = 1;
    }
  }
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint64_t))) = uVar4;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint64_t))) = ((long)(int)param_2 & 0xfU) << 0xf | ((long)(iVar10 + 0x3f >> 6) & 0x3fU) << 9;
  if ((*(int16_t*)rtguest::G((uint32_t)psVar3 + (uint32_t)(1) * sizeof(int16_t))) == 2) {
    if (sVar2 != 1) {
      iVar1 = (iVar10 + 0x9ff) / iVar10;
      iVar11 = iVar11 + 0x19;
      if (iVar10 == 0) {
        trap(7);
      }
      uVar8 = (long)(iVar13 * iVar1) + 0x27c;
LAB_0026f9b0:
      uVar8 = (long)(iVar1 + -1) << 0x17 | (long)(iVar1 * iVar10 + -1) << 0x20 |
              uVar8 & 0xfff | (long)(iVar12 + -1) << 0x2c | ((long)iVar11 & 0xfffU) << 0xc;
      (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint64_t))) = uVar8;
      goto LAB_0026f9e0;
    }
    iVar1 = (iVar10 + 0x9ff) / iVar10;
    if (iVar10 == 0) {
      trap(7);
    }
    uVar8 = ((long)(iVar11 + 0x32) & 0xfffU) << 0xc;
    uVar9 = (long)(iVar1 + -1) << 0x17;
    uVar7 = (long)(iVar13 * iVar1) + 0x27cU & 0xfff;
    uVar6 = (long)(iVar1 * iVar10 + -1) << 0x20;
    if ((*(int16_t*)rtguest::G((uint32_t)psVar3 + (uint32_t)(2) * sizeof(int16_t))) == 0) {
LAB_0026f95c:
      lVar5 = (long)(iVar12 + -1);
      uVar9 = uVar9 | uVar6;
    }
    else {
      uVar9 = uVar9 | uVar6;
      lVar5 = (long)(iVar12 * 2 + -1);
    }
  }
  else {
    if ((*(int16_t*)rtguest::G((uint32_t)psVar3 + (uint32_t)(1) * sizeof(int16_t))) != 3) {
      iVar11 = ::printf((char *)rtguest::G((uint32_t)(0x330110u)));
      uVar8 = (uint32_t)iVar11;
      goto LAB_0026f9e0;
    }
    if (sVar2 != 1) {
      iVar1 = (iVar10 + 0x9ff) / iVar10;
      iVar11 = iVar11 + 0x24;
      if (iVar10 == 0) {
        trap(7);
      }
      uVar8 = (long)(iVar13 * iVar1) + 0x290;
      goto LAB_0026f9b0;
    }
    iVar1 = (iVar10 + 0x9ff) / iVar10;
    if (iVar10 == 0) {
      trap(7);
    }
    uVar8 = ((long)(iVar11 + 0x48) & 0xfffU) << 0xc;
    uVar9 = (long)(iVar1 + -1) << 0x17;
    uVar7 = (long)(iVar13 * iVar1) + 0x290U & 0xfff;
    uVar6 = (long)(iVar1 * iVar10 + -1) << 0x20;
    if ((*(int16_t*)rtguest::G((uint32_t)psVar3 + (uint32_t)(2) * sizeof(int16_t))) == 0) goto LAB_0026f95c;
    uVar9 = uVar9 | uVar6;
    lVar5 = (long)(iVar12 * 2 + -1);
  }
  uVar8 = uVar9 | uVar7 | lVar5 << 0x2c | uVar8;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint64_t))) = uVar8;
LAB_0026f9e0:
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(uint64_t))) = 0;
  return uVar8;
}


RT_GUEST_FN(0x0026f798, guest::sceGsSetDefDispEnv)

}  // namespace guest
