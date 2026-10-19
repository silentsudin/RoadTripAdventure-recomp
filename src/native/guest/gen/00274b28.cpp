// guest:0x00274b28 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __ieee754_rem_pio2(uint32_t param_1, uint32_t param_2, ...) {
  undefined8 uVar1;
  double dVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  long lVar6;
  double dVar7;
  int iVar8;
uint32_t puVar9;
  ulong uVar10;
  undefined8 uVar11;
uint32_t puVar12;
uint32_t pdVar13;
  int iVar14;
  ulong uVar15;
uint32_t auStack_70 = rtguest::allocFrame(16);
uint32_t auStack_60 = rtguest::allocFrame(16);
  
  puVar12 = auStack_70;
  uVar15 = (long)param_1 >> 0x20;
  uVar10 = uVar15 & 0x7fffffff;
  pdVar13 = (uint32_t)param_2;
  if (uVar10 < 0x3fe921fc) {
    (*(double*)rtguest::G((uint32_t)pdVar13)) = param_1;
    (*(double*)rtguest::G((uint32_t)pdVar13 + (uint32_t)(1) * sizeof(double))) = 0.0;
    return 0;
  }
  if (uVar10 < 0x4002d97c) {
    if (0 < (long)uVar15) {
      uVar1 = dpsub(param_1,0x3ff921fb54400000);
      if (uVar10 == 0x3ff921fb) {
        uVar11 = 0x3ba3198a2e037073;
        uVar1 = dpsub(uVar1,0x3dd0b4611a600000);
      }
      else {
        uVar11 = 0x3dd0b4611a626331;
      }
      dVar2 = (double)dpsub(uVar1,uVar11);
      (*(double*)rtguest::G((uint32_t)pdVar13)) = dVar2;
      uVar1 = dpsub(uVar1,dVar2);
      dVar2 = (double)dpsub(uVar1,uVar11);
      (*(double*)rtguest::G((uint32_t)pdVar13 + (uint32_t)(1) * sizeof(double))) = dVar2;
      return 1;
    }
    uVar1 = dpadd(param_1,0x3ff921fb54400000);
    if (uVar10 == 0x3ff921fb) {
      uVar11 = 0x3ba3198a2e037073;
      uVar1 = dpadd(uVar1,0x3dd0b4611a600000);
    }
    else {
      uVar11 = 0x3dd0b4611a626331;
    }
    dVar2 = (double)dpadd(uVar1,uVar11);
    (*(double*)rtguest::G((uint32_t)pdVar13)) = dVar2;
    uVar1 = dpsub(uVar1,dVar2);
    dVar2 = (double)dpadd(uVar1,uVar11);
    (*(double*)rtguest::G((uint32_t)pdVar13 + (uint32_t)(1) * sizeof(double))) = dVar2;
    return -1;
  }
  iVar14 = (int)uVar10 >> 0x14;
  if (0x413921fb < uVar10) {
    if (0x7fefffff < uVar10) {
      dVar2 = (double)dpsub();
      (*(double*)rtguest::G((uint32_t)pdVar13)) = dVar2;
      (*(double*)rtguest::G((uint32_t)pdVar13 + (uint32_t)(1) * sizeof(double))) = dVar2;
      return 0;
    }
    uVar10 = (uint32_t)param_1 & 0xffffffff |
             (long)((int)uVar10 + (iVar14 + -0x416) * -0x100000) << 0x20;
    iVar8 = 1;
    do {
      iVar8 = iVar8 + -1;
      uVar1 = dptoli(uVar10);
      uVar1 = litodp(uVar1);
      (*(uint64_t*)rtguest::G((uint32_t)puVar12)) = uVar1;
      puVar12 = puVar12 + 1;
      uVar1 = dpsub(uVar10,uVar1);
      uVar10 = dpmul(uVar1,0x4170000000000000);
    } while (-1 < iVar8);
    iVar8 = 3;
    (*(uint64_t*)rtguest::G((uint32_t)auStack_60 + (uint32_t)(0) * 8)) = uVar10;
    puVar9 = auStack_60;
    while( true ) {
      uVar10 = (*(uint64_t*)rtguest::G((uint32_t)puVar9));
      puVar9 = puVar9 + -1;
      lVar6 = dpcmp(uVar10,0);
      if (lVar6 != 0) break;
      iVar8 = iVar8 + -1;
    }
    iVar8 = __kernel_rem_pio2(auStack_70,param_2,iVar14 + -0x416,iVar8,2,0x3312f8);
    if (-1 < (long)uVar15) {
      return iVar8;
    }
    dVar7 = (double)dpsub(0,(*(double*)rtguest::G((uint32_t)pdVar13)));
    dVar2 = (*(double*)rtguest::G((uint32_t)pdVar13 + (uint32_t)(1) * sizeof(double)));
    (*(double*)rtguest::G((uint32_t)pdVar13)) = dVar7;
    goto LAB_002750a0;
  }
  dVar2 = fabs(param_1);
  uVar1 = dpmul(dVar2,0x3fe45f306dc9c883);
  uVar1 = dpadd(uVar1,0x3fe0000000000000);
  iVar8 = dptoli(uVar1);
  uVar1 = litodp(iVar8);
  uVar11 = dpmul(uVar1,0x3ff921fb54400000);
  uVar11 = dpsub(dVar2,uVar11);
  uVar3 = dpmul(uVar1,0x3dd0b4611a626331);
  if ((iVar8 < 0x20) && (uVar10 != (long)(int)(*(uint32_t*)rtguest::G((uint32_t)(0x3313fcu + (uint32_t)(iVar8) * 4))))) {
LAB_00274f1c:
    dVar2 = (double)dpsub(uVar11,uVar3);
    (*(double*)rtguest::G((uint32_t)pdVar13)) = dVar2;
    uVar4 = uVar11;
  }
  else {
    dVar2 = (double)dpsub(uVar11,uVar3);
    (*(double*)rtguest::G((uint32_t)pdVar13)) = dVar2;
    uVar4 = uVar11;
    if (0x10 < (int)(iVar14 - ((uint32_t)((uint32_t)dVar2 >> 0x34) & 0x7ff))) {
      uVar3 = dpmul(uVar1,0x3dd0b4611a600000);
      uVar4 = dpsub(uVar11,uVar3);
      uVar5 = dpmul(uVar1,0x3ba3198a2e037073);
      uVar11 = dpsub(uVar11,uVar4);
      uVar11 = dpsub(uVar11,uVar3);
      uVar3 = dpsub(uVar5,uVar11);
      dVar2 = (double)dpsub(uVar4,uVar3);
      (*(double*)rtguest::G((uint32_t)pdVar13)) = dVar2;
      if (0x31 < (int)(iVar14 - ((uint32_t)((uint32_t)dVar2 >> 0x34) & 0x7ff))) {
        uVar3 = dpmul(uVar1,0x3ba3198a2e000000);
        uVar11 = dpsub(uVar4,uVar3);
        uVar1 = dpmul(uVar1,0x397b839a252049c1);
        uVar4 = dpsub(uVar4,uVar11);
        uVar3 = dpsub(uVar4,uVar3);
        uVar3 = dpsub(uVar1,uVar3);
        goto LAB_00274f1c;
      }
    }
  }
  dVar7 = (*(double*)rtguest::G((uint32_t)pdVar13));
  uVar1 = dpsub(uVar4,dVar7);
  dVar2 = (double)dpsub(uVar1,uVar3);
  (*(double*)rtguest::G((uint32_t)pdVar13 + (uint32_t)(1) * sizeof(double))) = dVar2;
  if (-1 < (long)uVar15) {
    return iVar8;
  }
  dVar7 = (double)dpsub(0,dVar7);
  (*(double*)rtguest::G((uint32_t)pdVar13)) = dVar7;
LAB_002750a0:
  dVar2 = (double)dpsub(0,dVar2);
  (*(double*)rtguest::G((uint32_t)pdVar13 + (uint32_t)(1) * sizeof(double))) = dVar2;
  return -iVar8;
}


RT_GUEST_FN(0x00274b28, guest::__ieee754_rem_pio2)

}  // namespace guest
