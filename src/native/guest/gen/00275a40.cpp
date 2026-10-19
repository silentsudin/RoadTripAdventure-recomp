// guest:0x00275a40 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __kernel_rem_pio2(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
uint32_t auStack_2c4 = rtguest::allocFrame(84);
uint32_t auStack_270 = rtguest::allocFrame(152);
uint32_t auStack_1d8 = rtguest::allocFrame(16);
uint32_t auStack_1c8 = rtguest::allocFrame(152);
uint32_t auStack_130 = rtguest::allocFrame(160);
rtguest::allocFrame(8);  // guest frame gap
    uint32_t iStack_88 = rtguest::allocFrame(4);
    uint32_t iStack_84 = rtguest::allocFrame(4);
    uint32_t iStack_80 = rtguest::allocFrame(4);
    uint32_t iStack_7c = rtguest::allocFrame(4);
    uint32_t iStack_78 = rtguest::allocFrame(4);
    uint32_t uStack_74 = rtguest::allocFrame(4);
    uint32_t iStack_70 = rtguest::allocFrame(4);
    rtguest::allocFrame(12);  // guest frame gap
    uint32_t uStack_60 = rtguest::allocFrame(4);
    uint32_t uStack_5c = rtguest::allocFrame(4);
  uint uVar1;
  bool bVar2;
  undefined4 uVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  double dVar7;
  long lVar8;
uint32_t puVar9;
  int iVar10;
  undefined8 uVar11;
  undefined8 uVar12;
  int iVar13;
  int iVar14;
uint32_t puVar15;
uint32_t puVar16;
uint32_t puVar17;
uint32_t puVar18;
  int iVar19;
uint32_t puVar20;
  undefined8 uVar21;
uint32_t puStack_90;
uint32_t puStack_8c;
  
  (*(int32_t*)rtguest::G((uint32_t)iStack_7c)) = (param_3 + -3) / 0x18;
  (*(int32_t*)rtguest::G((uint32_t)iStack_80)) = param_4 + -1;
  (*(int32_t*)rtguest::G((uint32_t)iStack_78)) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x00331990 + param_5 * 4)));
  (*(int32_t*)rtguest::G((uint32_t)iStack_88)) = param_5;
  puStack_90 = param_1;
  iVar10 = (*(int32_t*)rtguest::G((uint32_t)iStack_80)) + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x00331990 + param_5 * 4)));
  puStack_8c = param_2;
  (*(int32_t*)rtguest::G((uint32_t)iStack_84)) = param_6;
  if ((*(int32_t*)rtguest::G((uint32_t)iStack_7c)) < 0) {
    (*(int32_t*)rtguest::G((uint32_t)iStack_7c)) = 0;
  }
  iVar13 = (*(int32_t*)rtguest::G((uint32_t)iStack_7c)) - (*(int32_t*)rtguest::G((uint32_t)iStack_80));
  (*(int32_t*)rtguest::G((uint32_t)iStack_70)) = param_3 + (*(int32_t*)rtguest::G((uint32_t)iStack_7c)) * -0x18 + -0x18;
  if (-1 < iVar10) {
    iVar10 = iVar10 + 1;
    puVar20 = (uint32_t)(iVar13 * 4 + param_6);
    puVar15 = auStack_270;
    do {
      if (iVar13 < 0) {
        (*(uint64_t*)rtguest::G((uint32_t)puVar15)) = 0;
      }
      else {
        uVar21 = litodp((*(uint32_t*)rtguest::G((uint32_t)puVar20)));
        (*(uint64_t*)rtguest::G((uint32_t)puVar15)) = uVar21;
      }
      iVar10 = iVar10 + -1;
      puVar15 = puVar15 + 1;
      puVar20 = puVar20 + 1;
      iVar13 = iVar13 + 1;
    } while (iVar10 != 0);
  }
  iVar13 = 0;
  iVar10 = (*(int32_t*)rtguest::G((uint32_t)iStack_78));
  if (-1 < (*(int32_t*)rtguest::G((uint32_t)iStack_78))) {
    puVar15 = auStack_130;
    do {
      uVar21 = 0;
      if (-1 < (*(int32_t*)rtguest::G((uint32_t)iStack_80))) {
        puVar16 = auStack_270 + (*(int32_t*)rtguest::G((uint32_t)iStack_80)) + iVar13;
        iVar10 = (*(int32_t*)rtguest::G((uint32_t)iStack_80)) + 1;
        puVar18 = puStack_90;
        do {
          uVar11 = (*(uint64_t*)rtguest::G((uint32_t)puVar18));
          puVar18 = puVar18 + 1;
          uVar12 = (*(uint64_t*)rtguest::G((uint32_t)puVar16));
          puVar16 = puVar16 + -1;
          iVar10 = iVar10 + -1;
          uVar11 = dpmul(uVar11,uVar12);
          uVar21 = dpadd(uVar21,uVar11);
        } while (iVar10 != 0);
      }
      (*(uint64_t*)rtguest::G((uint32_t)puVar15)) = uVar21;
      iVar13 = iVar13 + 1;
      puVar15 = puVar15 + 1;
      iVar10 = (*(int32_t*)rtguest::G((uint32_t)iStack_78));
    } while (iVar13 <= (*(int32_t*)rtguest::G((uint32_t)iStack_78)));
  }
LAB_00275bc8:
  puVar15 = auStack_130 + iVar10;
  uVar21 = (*(uint64_t*)rtguest::G((uint32_t)puVar15));
  puVar9 = auStack_2c4 + 1;
  for (iVar13 = iVar10; 0 < iVar13; iVar13 = iVar13 + -1) {
    puVar15 = puVar15 + -1;
    uVar11 = dpmul(uVar21,0x3e70000000000000);
    uVar11 = dptoli(uVar11);
    uVar11 = litodp(uVar11);
    uVar12 = dpmul(uVar11,0x4170000000000000);
    uVar21 = dpsub(uVar21,uVar12);
    uVar3 = dptoli(uVar21);
    uVar21 = (*(uint64_t*)rtguest::G((uint32_t)puVar15));
    (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = uVar3;
    puVar9 = puVar9 + 1;
    uVar21 = dpadd(uVar21,uVar11);
  }
  uVar21 = scalbn(uVar21,(*(int32_t*)rtguest::G((uint32_t)iStack_70)));
  dVar7 = (double)dpmul(uVar21,0x3fc0000000000000);
  dVar7 = floor(dVar7);
  uVar11 = dpmul(dVar7,0x4020000000000000);
  uVar21 = dpsub(uVar21,uVar11);
  uVar11 = dptoli(uVar21);
  (*(uint32_t*)rtguest::G((uint32_t)uStack_74)) = (uint32_t)uVar11;
  uVar11 = litodp(uVar11);
  uVar21 = dpsub(uVar21,uVar11);
  if ((*(int32_t*)rtguest::G((uint32_t)iStack_70)) < 1) {
    if ((*(int32_t*)rtguest::G((uint32_t)iStack_70)) == 0) {
      iVar13 = (int)(*(uint32_t*)rtguest::G((uint32_t)auStack_2c4 + (uint32_t)(iVar10) * 4)) >> 0x17;
    }
    else {
      iVar13 = 2;
      lVar8 = dpcmp(uVar21,0x3fe0000000000000);
      if (lVar8 < 0) {
        iVar13 = 0;
      }
    }
  }
  else {
    uVar4 = (*(uint32_t*)rtguest::G((uint32_t)auStack_2c4 + (uint32_t)(iVar10) * 4));
    iVar13 = (int)uVar4 >> (0x18U - (*(int32_t*)rtguest::G((uint32_t)iStack_70)) & 0x1f);
    (*(uint32_t*)rtguest::G((uint32_t)uStack_74)) = (*(uint32_t*)rtguest::G((uint32_t)uStack_74)) + iVar13;
    uVar4 = uVar4 - (iVar13 << (0x18U - (*(int32_t*)rtguest::G((uint32_t)iStack_70)) & 0x1f));
    iVar13 = (int)uVar4 >> (0x17U - (*(int32_t*)rtguest::G((uint32_t)iStack_70)) & 0x1f);
    (*(uint32_t*)rtguest::G((uint32_t)auStack_2c4 + (uint32_t)(iVar10) * 4)) = uVar4;
  }
  if (0 < iVar13) {
    bVar2 = false;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_74)) = (*(uint32_t*)rtguest::G((uint32_t)uStack_74)) + 1;
    puVar9 = auStack_2c4 + 1;
    iVar19 = iVar10;
    if (0 < iVar10) {
      do {
        iVar14 = (*(uint32_t*)rtguest::G((uint32_t)puVar9));
        if (bVar2) {
          iVar5 = 0xffffff - iVar14;
LAB_00275dbc:
          (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = iVar5;
        }
        else {
          iVar5 = 0x1000000 - iVar14;
          if (iVar14 != 0) {
            bVar2 = true;
            goto LAB_00275dbc;
          }
        }
        iVar19 = iVar19 + -1;
        puVar9 = puVar9 + 1;
      } while (iVar19 != 0);
    }
    if (0 < (*(int32_t*)rtguest::G((uint32_t)iStack_70))) {
      if ((*(int32_t*)rtguest::G((uint32_t)iStack_70)) == 1) {
        uVar4 = 0x7f0000;
      }
      else {
        if ((*(int32_t*)rtguest::G((uint32_t)iStack_70)) != 2) goto LAB_00275e30;
        uVar4 = 0x3f0000;
      }
      (*(uint32_t*)rtguest::G((uint32_t)auStack_2c4 + (uint32_t)(iVar10) * 4)) = (*(uint32_t*)rtguest::G((uint32_t)auStack_2c4 + (uint32_t)(iVar10) * 4)) & (uVar4 | 0xffff);
    }
LAB_00275e30:
    if ((iVar13 == 2) && (uVar21 = dpsub(0x3ff0000000000000,uVar21), bVar2)) {
      uVar11 = scalbn(0x3ff0000000000000,(*(int32_t*)rtguest::G((uint32_t)iStack_70)));
      uVar21 = dpsub(uVar21,uVar11);
    }
  }
  lVar8 = dpcmp(uVar21,0);
  if (lVar8 == 0) {
    iVar19 = iVar10 + -1;
    uVar4 = 0;
    if ((*(int32_t*)rtguest::G((uint32_t)iStack_78)) <= iVar19) {
      puVar9 = auStack_2c4 + iVar10;
      do {
        iVar19 = iVar19 + -1;
        uVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar9));
        puVar9 = puVar9 + -1;
        uVar4 = uVar4 | uVar1;
      } while ((*(int32_t*)rtguest::G((uint32_t)iStack_78)) <= iVar19);
    }
    if (uVar4 != 0) goto LAB_00275ff0;
    iVar13 = 1;
    if ((*(uint32_t*)rtguest::G((uint32_t)auStack_2c4 + (uint32_t)(iStack_78) * 4)) == 0) {
      puVar9 = auStack_2c4 + (*(int32_t*)rtguest::G((uint32_t)iStack_78));
      do {
        puVar9 = puVar9 + -1;
        iVar13 = iVar13 + 1;
      } while ((*(uint32_t*)rtguest::G((uint32_t)puVar9)) == 0);
    }
    iVar13 = iVar10 + iVar13;
    iVar19 = iVar10 + 1;
    iVar10 = iVar13;
    if (iVar19 <= iVar13) {
      puVar15 = auStack_270 + (*(int32_t*)rtguest::G((uint32_t)iStack_80)) + iVar19;
      puVar18 = auStack_130 + iVar19;
      puVar20 = (uint32_t)(((*(int32_t*)rtguest::G((uint32_t)iStack_7c)) + iVar19) * 4 + (*(int32_t*)rtguest::G((uint32_t)iStack_84)));
      do {
        uVar11 = 0;
        iVar14 = (*(int32_t*)rtguest::G((uint32_t)iStack_80)) + iVar19;
        uVar21 = litodp((*(uint32_t*)rtguest::G((uint32_t)puVar20)));
        (*(uint64_t*)rtguest::G((uint32_t)puVar15)) = uVar21;
        if (-1 < (*(int32_t*)rtguest::G((uint32_t)iStack_80))) {
          iVar5 = (*(int32_t*)rtguest::G((uint32_t)iStack_80)) + 1;
          puVar17 = auStack_270 + iVar14;
          puVar16 = puStack_90;
          do {
            uVar21 = (*(uint64_t*)rtguest::G((uint32_t)puVar16));
            puVar16 = puVar16 + 1;
            uVar12 = (*(uint64_t*)rtguest::G((uint32_t)puVar17));
            puVar17 = puVar17 + -1;
            iVar5 = iVar5 + -1;
            uVar21 = dpmul(uVar21,uVar12);
            uVar11 = dpadd(uVar11,uVar21);
          } while (iVar5 != 0);
        }
        iVar19 = iVar19 + 1;
        (*(uint64_t*)rtguest::G((uint32_t)puVar18)) = uVar11;
        puVar15 = puVar15 + 1;
        puVar18 = puVar18 + 1;
        puVar20 = puVar20 + 1;
      } while (iVar19 <= iVar13);
    }
    goto LAB_00275bc8;
  }
LAB_00275ff0:
  lVar8 = dpcmp(uVar21,0);
  if (lVar8 == 0) {
    puVar9 = auStack_2c4 + iVar10;
    (*(int32_t*)rtguest::G((uint32_t)iStack_70)) = (*(int32_t*)rtguest::G((uint32_t)iStack_70)) + -0x18;
    uVar4 = (*(uint32_t*)rtguest::G((uint32_t)puVar9));
    while (iVar10 = iVar10 + -1, uVar4 == 0) {
      puVar9 = puVar9 + -1;
      uVar4 = (*(uint32_t*)rtguest::G((uint32_t)puVar9));
      (*(int32_t*)rtguest::G((uint32_t)iStack_70)) = (*(int32_t*)rtguest::G((uint32_t)iStack_70)) + -0x18;
    }
  }
  else {
    uVar21 = scalbn(uVar21,-(*(int32_t*)rtguest::G((uint32_t)iStack_70)));
    lVar8 = dpcmp(uVar21,0x4170000000000000);
    if (lVar8 < 0) {
      iVar19 = iVar10 << 2;
    }
    else {
      (*(int32_t*)rtguest::G((uint32_t)iStack_70)) = (*(int32_t*)rtguest::G((uint32_t)iStack_70)) + 0x18;
      uVar11 = dpmul(uVar21,0x3e70000000000000);
      uVar11 = dptoli(uVar11);
      uVar11 = litodp(uVar11);
      iVar14 = iVar10 + 1;
      uVar12 = dpmul(uVar11,0x4170000000000000);
      iVar10 = iVar10 + 1;
      uVar21 = dpsub(uVar21,uVar12);
      uVar4 = dptoli(uVar21);
      iVar19 = iVar10 * 4;
      (*(uint32_t*)rtguest::G((uint32_t)auStack_2c4 + (uint32_t)(iVar14) * 4)) = uVar4;
      uVar21 = uVar11;
    }
    uVar3 = dptoli(uVar21);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)auStack_2c4 + iVar19 + 4))) = uVar3;
  }
  uVar21 = scalbn(0x3ff0000000000000,(*(int32_t*)rtguest::G((uint32_t)iStack_70)));
  if (-1 < iVar10) {
    puVar9 = auStack_2c4 + iVar10 + 1;
    puVar15 = auStack_130 + iVar10;
    uVar4 = (*(uint32_t*)rtguest::G((uint32_t)puVar9));
    iVar19 = iVar10;
    while( true ) {
      puVar9 = puVar9 + -1;
      iVar19 = iVar19 + -1;
      uVar11 = litodp(uVar4);
      uVar11 = dpmul(uVar21,uVar11);
      (*(uint64_t*)rtguest::G((uint32_t)puVar15)) = uVar11;
      puVar15 = puVar15 + -1;
      uVar21 = dpmul(uVar21,0x3e70000000000000);
      if (iVar19 < 0) break;
      uVar4 = (*(uint32_t*)rtguest::G((uint32_t)puVar9));
    }
  }
  if (-1 < iVar10) {
    lVar8 = (long)(int)(auStack_1d8 + 1);
    iVar19 = iVar10;
    do {
      uVar21 = 0;
      iVar14 = 0;
      if ((*(int32_t*)rtguest::G((uint32_t)iStack_78)) < 0) {
        iVar5 = iVar10 - iVar19;
LAB_00276234:
        iVar6 = iVar5 << 3;
      }
      else {
        iVar5 = iVar10 - iVar19;
        if (iVar5 < 0) goto LAB_00276234;
        puVar18 = 0x003319a0;
        puVar15 = auStack_130 + iVar19;
        do {
          uVar11 = (*(uint64_t*)rtguest::G((uint32_t)puVar18));
          puVar18 = puVar18 + 1;
          uVar12 = (*(uint64_t*)rtguest::G((uint32_t)puVar15));
          puVar15 = puVar15 + 1;
          (*(uint32_t*)rtguest::G((uint32_t)uStack_60)) = (uint32_t)lVar8;
          (*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) = (uint32_t)((uint32_t)lVar8 >> 0x20);
          iVar14 = iVar14 + 1;
          uVar11 = dpmul(uVar11,uVar12);
          uVar21 = dpadd(uVar21,uVar11);
          lVar8 = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_5c)),(*(uint32_t*)rtguest::G((uint32_t)uStack_60)));
          if ((*(int32_t*)rtguest::G((uint32_t)iStack_78)) < iVar14) goto LAB_00276234;
          iVar6 = iVar5 * 8;
        } while (iVar14 <= iVar5);
      }
      iVar19 = iVar19 + -1;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)lVar8 + iVar6))) = uVar21;
    } while (-1 < iVar19);
  }
  if ((*(int32_t*)rtguest::G((uint32_t)iStack_88)) < 3) {
    if ((*(int32_t*)rtguest::G((uint32_t)iStack_88)) < 1) {
      if ((*(int32_t*)rtguest::G((uint32_t)iStack_88)) == 0) {
        uVar21 = 0;
        if (-1 < iVar10) {
          puVar15 = auStack_1d8 + iVar10 + 1;
          do {
            uVar11 = (*(uint64_t*)rtguest::G((uint32_t)puVar15));
            puVar15 = puVar15 + -1;
            iVar10 = iVar10 + -1;
            uVar21 = dpadd(uVar21,uVar11);
          } while (-1 < iVar10);
        }
        (*(uint64_t*)rtguest::G((uint32_t)puStack_8c)) = uVar21;
        if (iVar13 != 0) {
          uVar21 = dpsub(0,uVar21);
          (*(uint64_t*)rtguest::G((uint32_t)puStack_8c)) = uVar21;
        }
      }
    }
    else {
      uVar21 = 0;
      if (-1 < iVar10) {
        puVar15 = auStack_1d8 + iVar10 + 1;
        iVar19 = iVar10;
        do {
          uVar11 = (*(uint64_t*)rtguest::G((uint32_t)puVar15));
          puVar15 = puVar15 + -1;
          iVar19 = iVar19 + -1;
          uVar21 = dpadd(uVar21,uVar11);
        } while (-1 < iVar19);
      }
      (*(uint64_t*)rtguest::G((uint32_t)puStack_8c)) = uVar21;
      if (iVar13 != 0) {
        uVar11 = dpsub(0,uVar21);
        (*(uint64_t*)rtguest::G((uint32_t)puStack_8c)) = uVar11;
      }
      uVar21 = dpsub((*(uint64_t*)rtguest::G((uint32_t)auStack_1d8 + (uint32_t)(1) * 8)),uVar21);
      if (0 < iVar10) {
        puVar15 = auStack_1d8 + 2;
        do {
          uVar11 = (*(uint64_t*)rtguest::G((uint32_t)puVar15));
          puVar15 = puVar15 + 1;
          iVar10 = iVar10 + -1;
          uVar21 = dpadd(uVar21,uVar11);
        } while (iVar10 != 0);
      }
      (*(uint64_t*)rtguest::G((uint32_t)puStack_8c + (uint32_t)(1) * sizeof(uint64_t))) = uVar21;
      if (iVar13 != 0) {
        uVar21 = dpsub(0,uVar21);
        (*(uint64_t*)rtguest::G((uint32_t)puStack_8c + (uint32_t)(1) * sizeof(uint64_t))) = uVar21;
      }
    }
  }
  else if ((*(int32_t*)rtguest::G((uint32_t)iStack_88)) == 3) {
    if (0 < iVar10) {
      puVar15 = auStack_1d8 + iVar10;
      iVar19 = iVar10;
      do {
        uVar11 = (*(uint64_t*)rtguest::G((uint32_t)puVar15));
        uVar12 = (*(uint64_t*)rtguest::G((uint32_t)puVar15 + (uint32_t)(1) * sizeof(uint64_t)));
        iVar19 = iVar19 + -1;
        uVar21 = dpadd(uVar11,uVar12);
        uVar11 = dpsub(uVar11,uVar21);
        uVar11 = dpadd(uVar12,uVar11);
        (*(uint64_t*)rtguest::G((uint32_t)puVar15)) = uVar21;
        (*(uint64_t*)rtguest::G((uint32_t)puVar15 + (uint32_t)(1) * sizeof(uint64_t))) = uVar11;
        puVar15 = puVar15 + -1;
      } while (0 < iVar19);
    }
    if (1 < iVar10) {
      puVar15 = auStack_1d8 + iVar10;
      iVar19 = iVar10;
      do {
        uVar11 = (*(uint64_t*)rtguest::G((uint32_t)puVar15));
        uVar12 = (*(uint64_t*)rtguest::G((uint32_t)puVar15 + (uint32_t)(1) * sizeof(uint64_t)));
        iVar19 = iVar19 + -1;
        uVar21 = dpadd(uVar11,uVar12);
        uVar11 = dpsub(uVar11,uVar21);
        uVar11 = dpadd(uVar12,uVar11);
        (*(uint64_t*)rtguest::G((uint32_t)puVar15)) = uVar21;
        (*(uint64_t*)rtguest::G((uint32_t)puVar15 + (uint32_t)(1) * sizeof(uint64_t))) = uVar11;
        puVar15 = puVar15 + -1;
      } while (1 < iVar19);
    }
    uVar21 = 0;
    if (1 < iVar10) {
      puVar15 = auStack_1d8 + iVar10 + 1;
      do {
        uVar11 = (*(uint64_t*)rtguest::G((uint32_t)puVar15));
        puVar15 = puVar15 + -1;
        iVar10 = iVar10 + -1;
        uVar21 = dpadd(uVar21,uVar11);
      } while (1 < iVar10);
    }
    if (iVar13 == 0) {
      (*(uint64_t*)rtguest::G((uint32_t)puStack_8c + (uint32_t)(2) * sizeof(uint64_t))) = uVar21;
      (*(uint64_t*)rtguest::G((uint32_t)puStack_8c)) = (*(uint64_t*)rtguest::G((uint32_t)auStack_1d8 + (uint32_t)(1) * 8));
      (*(uint64_t*)rtguest::G((uint32_t)puStack_8c + (uint32_t)(1) * sizeof(uint64_t))) = (*(uint64_t*)rtguest::G((uint32_t)auStack_1c8 + (uint32_t)(0) * 8));
    }
    else {
      uVar11 = dpsub(0,(*(uint64_t*)rtguest::G((uint32_t)auStack_1d8 + (uint32_t)(1) * 8)));
      (*(uint64_t*)rtguest::G((uint32_t)puStack_8c)) = uVar11;
      uVar11 = dpsub(0,(*(uint64_t*)rtguest::G((uint32_t)auStack_1c8 + (uint32_t)(0) * 8)));
      (*(uint64_t*)rtguest::G((uint32_t)puStack_8c + (uint32_t)(1) * sizeof(uint64_t))) = uVar11;
      uVar21 = dpsub(0,uVar21);
      (*(uint64_t*)rtguest::G((uint32_t)puStack_8c + (uint32_t)(2) * sizeof(uint64_t))) = uVar21;
    }
  }
  return (*(uint32_t*)rtguest::G((uint32_t)uStack_74)) & 7;
}


RT_GUEST_FN(0x00275a40, guest::__kernel_rem_pio2)

}  // namespace guest
