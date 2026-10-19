// guest:0x00281458 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _dtoa_r(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, uint32_t param_7, ...) {
    uint32_t iStack_60 = rtguest::allocFrame(4);
    uint32_t iStack_64 = rtguest::allocFrame(4);
    uint32_t iStack_68 = rtguest::allocFrame(4);
    uint32_t iStack_70 = rtguest::allocFrame(4);
    uint32_t iStack_78 = rtguest::allocFrame(4);
    uint32_t iStack_7c = rtguest::allocFrame(4);
    uint32_t iStack_80 = rtguest::allocFrame(4);
    uint32_t iStack_94 = rtguest::allocFrame(4);
    uint32_t iStack_98 = rtguest::allocFrame(4);
    uint32_t iStack_ac = rtguest::allocFrame(4);
    uint32_t iStack_b0 = rtguest::allocFrame(4);
    uint32_t uStack_6c = rtguest::allocFrame(4);
    uint32_t uStack_74 = rtguest::allocFrame(4);
    uint32_t uStack_84 = rtguest::allocFrame(4);
    uint32_t uStack_88 = rtguest::allocFrame(4);
    uint32_t uStack_8c = rtguest::allocFrame(4);
    uint32_t uStack_90 = rtguest::allocFrame(4);
    uint32_t uStack_a4 = rtguest::allocFrame(4);
    uint32_t uStack_a8 = rtguest::allocFrame(4);
  bool bVar1;
  bool bVar2;
  undefined4 uVar3;
  long lVar4;
  undefined8 uVar5;
  undefined8 uVar6;
  ulong uVar7;
  undefined8 uVar8;
  int iVar9;
  char cVar10;
  ulong uVar11;
  uint uVar12;
  int iVar13;
  long lVar14;
uint32_t puVar15;
  int iVar16;
  uint uVar17;
  int iVar18;
uint32_t pcVar19;
uint32_t pcVar20;
uint32_t pcVar21;
  uint uVar22;
uint32_t piStack_a0;
uint32_t puStack_9c;
  
  iVar16 = (int)param_1;
  iVar13 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar16 + 0x40)));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_a8)) = param_3;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_a4)) = param_4;
  piStack_a0 = param_5;
  puStack_9c = param_7;
  if (iVar13 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 4))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar16 + 0x44)));
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 8))) = 1 << ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar16 + 0x44))) & 0x1f);
    _Bfree(param_1,iVar13);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar16 + 0x40))) = 0;
  }
  uVar11 = (uint32_t)(int)(param_2 >> 0x20);
  if ((long)uVar11 < 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_6)) = 1;
    param_2 = param_2 & 0xffffffff | (uVar11 & 0x7fffffff) << 0x20;
  }
  else {
    (*(uint32_t*)rtguest::G((uint32_t)param_6)) = 0;
  }
  uVar12 = (uint32_t)(param_2 >> 0x20);
  if ((uVar12 & 0x7ff00000) == 0x7ff00000) {
    (*(int32_t*)rtguest::G((uint32_t)piStack_a0)) = 9999;
    if ((param_2 & 0xfffffffffffff) == 0) {
      pcVar19 = 0x332380u;
    }
    else {
      pcVar19 = 0x332198u;
    }
    if (puStack_9c == (uint32_t)0x0) {
      return pcVar19;
    }
    pcVar20 = pcVar19 + 8;
    if ((*(int8_t*)rtguest::G((uint32_t)pcVar19 + (uint32_t)(3) * sizeof(int8_t))) == '\0') {
      pcVar20 = pcVar19 + 3;
    }
    (*(uint32_t*)rtguest::G((uint32_t)puStack_9c)) = pcVar20;
    return pcVar19;
  }
  lVar4 = dpcmp(param_2,0);
  if (lVar4 == 0) {
    (*(int32_t*)rtguest::G((uint32_t)piStack_a0)) = 1;
    if (puStack_9c == (uint32_t)0x0) {
      return 0x200261u;
    }
    (*(uint32_t*)rtguest::G((uint32_t)puStack_9c)) = 0x00332399;
    return 0x200261u;
  }
  (*(uint32_t*)rtguest::G((uint32_t)uStack_6c)) = _d2b(param_1,param_2,iStack_b0,iStack_ac);
  iVar13 = (*(int32_t*)rtguest::G((uint32_t)iStack_ac));
  uVar17 = uVar12 >> 0x14 & 0x7ff;
  if (uVar17 == 0) {
    iVar9 = (*(int32_t*)rtguest::G((uint32_t)iStack_ac)) + (*(int32_t*)rtguest::G((uint32_t)iStack_b0));
    iVar18 = iVar9 + 0x432;
    if (iVar18 < 0x21) {
      uVar12 = (uint32_t)param_2 << (-iVar18 & 0x1fU);
    }
    else {
      uVar12 = uVar12 << (-iVar18 & 0x1fU) | (uint32_t)param_2 >> (iVar9 + 0x412U & 0x1f);
    }
    uVar11 = litodp(uVar12);
    if ((int)uVar12 < 0) {
      uVar11 = dpadd(uVar11,0x41f0000000000000);
    }
    (*(int32_t*)rtguest::G((uint32_t)iStack_70)) = 1;
    uVar17 = iVar9 - 1;
    uVar11 = uVar11 & 0xffffffff | (long)((int)(uVar11 >> 0x20) + -0x1f00000) << 0x20;
  }
  else {
    uVar17 = uVar17 - 0x3ff;
    (*(int32_t*)rtguest::G((uint32_t)iStack_70)) = 0;
    uVar11 = param_2 & 0xffffffff |
             ((long)(param_2 & 0xffffffff | ((long)param_2 >> 0x20 & 0xfffffU) << 0x20) >> 0x20 |
             0x3ff00000U) << 0x20;
  }
  uVar5 = dpsub(uVar11,0x3ff8000000000000);
  uVar5 = dpmul(uVar5,0x3fd287a7636f4361);
  uVar5 = dpadd(uVar5,0x3fc68a288b60c8b3);
  uVar6 = litodp(uVar17);
  uVar6 = dpmul(uVar6,0x3fd34413509f79fb);
  uVar5 = dpadd(uVar5,uVar6);
  uVar12 = dptoli(uVar5);
  lVar4 = dpcmp(uVar5,0);
  if (lVar4 < 0) {
    uVar6 = litodp(uVar12);
    lVar4 = dpcmp(uVar5,uVar6);
    if (lVar4 != 0) {
      uVar12 = uVar12 - 1;
    }
  }
  (*(int32_t*)rtguest::G((uint32_t)iStack_80)) = 1;
  if (uVar12 < 0x17) {
    (*(int32_t*)rtguest::G((uint32_t)iStack_80)) = 0;
    lVar4 = dpcmp(param_2,(*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x00332430 + uVar12 * 8))));
    if (lVar4 < 0) {
      uVar12 = uVar12 - 1;
    }
  }
  (*(int32_t*)rtguest::G((uint32_t)iStack_78)) = (iVar13 - uVar17) + -1;
  if ((*(int32_t*)rtguest::G((uint32_t)iStack_78)) < 0) {
    (*(int32_t*)rtguest::G((uint32_t)iStack_98)) = -(*(int32_t*)rtguest::G((uint32_t)iStack_78));
    (*(int32_t*)rtguest::G((uint32_t)iStack_78)) = 0;
  }
  else {
    (*(int32_t*)rtguest::G((uint32_t)iStack_98)) = 0;
  }
  if ((int)uVar12 < 0) {
    (*(int32_t*)rtguest::G((uint32_t)iStack_94)) = -uVar12;
    (*(int32_t*)rtguest::G((uint32_t)iStack_98)) = (*(int32_t*)rtguest::G((uint32_t)iStack_98)) - uVar12;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_74)) = 0;
  }
  else {
    (*(int32_t*)rtguest::G((uint32_t)iStack_94)) = 0;
    (*(int32_t*)rtguest::G((uint32_t)iStack_78)) = (*(int32_t*)rtguest::G((uint32_t)iStack_78)) + uVar12;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_74)) = uVar12;
  }
  if (9 < (*(uint32_t*)rtguest::G((uint32_t)uStack_a8))) {
    (*(uint32_t*)rtguest::G((uint32_t)uStack_a8)) = 0;
  }
  bVar1 = 5 < (int)(*(uint32_t*)rtguest::G((uint32_t)uStack_a8));
  if (bVar1) {
    (*(uint32_t*)rtguest::G((uint32_t)uStack_a8)) = (*(uint32_t*)rtguest::G((uint32_t)uStack_a8)) - 4;
  }
  (*(int32_t*)rtguest::G((uint32_t)iStack_7c)) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_88)) = 0xffffffff;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_90)) = 0xffffffff;
  uVar22 = (*(uint32_t*)rtguest::G((uint32_t)uStack_a4));
  switch((*(uint32_t*)rtguest::G((uint32_t)uStack_a8))) {
  case 0:
  case 1:
    (*(uint32_t*)rtguest::G((uint32_t)uStack_a4)) = 0;
    uVar17 = 0x12;
    uVar22 = (*(uint32_t*)rtguest::G((uint32_t)uStack_a4));
    break;
  case 2:
    (*(int32_t*)rtguest::G((uint32_t)iStack_7c)) = 0;
  case 4:
    (*(uint32_t*)rtguest::G((uint32_t)uStack_90)) = 1;
    uVar17 = (*(uint32_t*)rtguest::G((uint32_t)uStack_90));
    uVar22 = (*(uint32_t*)rtguest::G((uint32_t)uStack_90));
    (*(uint32_t*)rtguest::G((uint32_t)uStack_88)) = (*(uint32_t*)rtguest::G((uint32_t)uStack_90));
    if (0 < (int)(*(uint32_t*)rtguest::G((uint32_t)uStack_a4))) {
      uVar17 = (*(uint32_t*)rtguest::G((uint32_t)uStack_a4));
      uVar22 = (*(uint32_t*)rtguest::G((uint32_t)uStack_a4));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_90)) = (*(uint32_t*)rtguest::G((uint32_t)uStack_a4));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_88)) = (*(uint32_t*)rtguest::G((uint32_t)uStack_a4));
    }
    break;
  case 3:
    (*(int32_t*)rtguest::G((uint32_t)iStack_7c)) = 0;
  case 5:
    (*(uint32_t*)rtguest::G((uint32_t)uStack_88)) = (*(uint32_t*)rtguest::G((uint32_t)uStack_a4)) + uVar12;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_90)) = (*(uint32_t*)rtguest::G((uint32_t)uStack_88)) + 1;
    uVar17 = (*(uint32_t*)rtguest::G((uint32_t)uStack_90));
    if ((int)(*(uint32_t*)rtguest::G((uint32_t)uStack_90)) < 1) {
      uVar17 = 1;
    }
  }
  (*(uint32_t*)rtguest::G((uint32_t)uStack_a4)) = uVar22;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar16 + 0x44))) = 0;
  iVar13 = 4;
  if (0x17 < uVar17) {
    iVar9 = 0;
    do {
      iVar13 = iVar13 * 2;
      iVar9 = iVar9 + 1;
    } while (iVar13 + 0x14U <= uVar17);
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar16 + 0x44))) = iVar9;
  }
  pcVar19 = (uint32_t)_Balloc(param_1,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar16 + 0x44))));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar16 + 0x40))) = pcVar19;
  uVar17 = (*(uint32_t*)rtguest::G((uint32_t)uStack_90));
  uVar22 = uVar12;
  if ((0xe < (*(uint32_t*)rtguest::G((uint32_t)uStack_90))) || (bVar1)) {
LAB_00281d84:
    (*(uint32_t*)rtguest::G((uint32_t)uStack_90)) = uVar17;
    if (((*(int32_t*)rtguest::G((uint32_t)iStack_b0)) < 0) || (0xe < (int)uVar22)) {
      bVar1 = (int)(*(uint32_t*)rtguest::G((uint32_t)uStack_a8)) < 2;
      (*(int32_t*)rtguest::G((uint32_t)iStack_68)) = 0;
      (*(int32_t*)rtguest::G((uint32_t)iStack_64)) = 0;
      iVar13 = (*(int32_t*)rtguest::G((uint32_t)iStack_98));
      iVar16 = (*(int32_t*)rtguest::G((uint32_t)iStack_94));
      if ((*(int32_t*)rtguest::G((uint32_t)iStack_7c)) != 0) {
        if (bVar1) {
          uVar12 = (*(int32_t*)rtguest::G((uint32_t)iStack_b0)) + 0x433;
          if ((*(int32_t*)rtguest::G((uint32_t)iStack_70)) == 0) {
            uVar12 = 0x36 - (*(int32_t*)rtguest::G((uint32_t)iStack_ac));
          }
        }
        else {
          iVar9 = (*(uint32_t*)rtguest::G((uint32_t)uStack_90)) - 1;
          iVar16 = (*(int32_t*)rtguest::G((uint32_t)iStack_94)) - iVar9;
          if ((*(int32_t*)rtguest::G((uint32_t)iStack_94)) < iVar9) {
            iVar16 = 0;
            iVar9 = iVar9 - (*(int32_t*)rtguest::G((uint32_t)iStack_94));
            (*(int32_t*)rtguest::G((uint32_t)iStack_94)) = (*(int32_t*)rtguest::G((uint32_t)iStack_94)) + iVar9;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_74)) = (*(uint32_t*)rtguest::G((uint32_t)uStack_74)) + iVar9;
          }
          uVar12 = (*(uint32_t*)rtguest::G((uint32_t)uStack_90));
          if ((int)(*(uint32_t*)rtguest::G((uint32_t)uStack_90)) < 0) {
            uVar12 = 0;
            iVar13 = (*(int32_t*)rtguest::G((uint32_t)iStack_98)) - (*(uint32_t*)rtguest::G((uint32_t)uStack_90));
          }
        }
        (*(int32_t*)rtguest::G((uint32_t)iStack_78)) = (*(int32_t*)rtguest::G((uint32_t)iStack_78)) + uVar12;
        (*(int32_t*)rtguest::G((uint32_t)iStack_64)) = _i2b(param_1,1);
        (*(int32_t*)rtguest::G((uint32_t)iStack_98)) = (*(int32_t*)rtguest::G((uint32_t)iStack_98)) + uVar12;
      }
      if ((0 < iVar13) && (0 < (*(int32_t*)rtguest::G((uint32_t)iStack_78)))) {
        iVar9 = (*(int32_t*)rtguest::G((uint32_t)iStack_78));
        if (iVar13 < (*(int32_t*)rtguest::G((uint32_t)iStack_78))) {
          iVar9 = iVar13;
        }
        (*(int32_t*)rtguest::G((uint32_t)iStack_78)) = (*(int32_t*)rtguest::G((uint32_t)iStack_78)) - iVar9;
        iVar13 = iVar13 - iVar9;
        (*(int32_t*)rtguest::G((uint32_t)iStack_98)) = (*(int32_t*)rtguest::G((uint32_t)iStack_98)) - iVar9;
      }
      if (0 < (*(int32_t*)rtguest::G((uint32_t)iStack_94))) {
        if ((*(int32_t*)rtguest::G((uint32_t)iStack_7c)) == 0) {
          (*(uint32_t*)rtguest::G((uint32_t)uStack_6c)) = _pow5mult(param_1,(*(uint32_t*)rtguest::G((uint32_t)uStack_6c)),(*(int32_t*)rtguest::G((uint32_t)iStack_94)));
        }
        else {
          if (0 < iVar16) {
            uVar5 = _pow5mult(param_1,(*(int32_t*)rtguest::G((uint32_t)iStack_64)),iVar16);
            (*(int32_t*)rtguest::G((uint32_t)iStack_64)) = (int)uVar5;
            uVar3 = _multiply(param_1,uVar5,(*(uint32_t*)rtguest::G((uint32_t)uStack_6c)));
            _Bfree(param_1,(*(uint32_t*)rtguest::G((uint32_t)uStack_6c)));
            (*(uint32_t*)rtguest::G((uint32_t)uStack_6c)) = uVar3;
          }
          if ((*(int32_t*)rtguest::G((uint32_t)iStack_94)) - iVar16 != 0) {
            (*(uint32_t*)rtguest::G((uint32_t)uStack_6c)) = _pow5mult(param_1,(*(uint32_t*)rtguest::G((uint32_t)uStack_6c)),(*(int32_t*)rtguest::G((uint32_t)iStack_94)) - iVar16);
          }
        }
      }
      uVar5 = _i2b(param_1,1);
      (*(int32_t*)rtguest::G((uint32_t)iStack_60)) = (int)uVar5;
      if (0 < (int)(*(uint32_t*)rtguest::G((uint32_t)uStack_74))) {
        (*(int32_t*)rtguest::G((uint32_t)iStack_60)) = _pow5mult(param_1,uVar5,(*(uint32_t*)rtguest::G((uint32_t)uStack_74)));
      }
      bVar2 = false;
      if (((bVar1) && ((param_2 & 0xfffffffffffff) == 0)) &&
         (((long)param_2 >> 0x20 & 0x7ff00000U) != 0)) {
        bVar2 = true;
        (*(int32_t*)rtguest::G((uint32_t)iStack_98)) = (*(int32_t*)rtguest::G((uint32_t)iStack_98)) + 1;
        (*(int32_t*)rtguest::G((uint32_t)iStack_78)) = (*(int32_t*)rtguest::G((uint32_t)iStack_78)) + 1;
      }
      if ((*(uint32_t*)rtguest::G((uint32_t)uStack_74)) == 0) {
        uVar12 = (*(int32_t*)rtguest::G((uint32_t)iStack_78)) + 1;
      }
      else {
        iVar16 = _hi0bits((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 0x10))) * 4 + (*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 0x10))));
        uVar12 = (*(int32_t*)rtguest::G((uint32_t)iStack_78)) - iVar16;
      }
      iVar16 = 0;
      if ((uVar12 & 0x1f) != 0) {
        iVar16 = 0x20 - (uVar12 & 0x1f);
      }
      if (iVar16 < 5) {
        if (iVar16 < 4) {
          iVar16 = iVar16 + 0x1c;
          goto LAB_002821c0;
        }
      }
      else {
        iVar16 = iVar16 + -4;
LAB_002821c0:
        (*(int32_t*)rtguest::G((uint32_t)iStack_78)) = (*(int32_t*)rtguest::G((uint32_t)iStack_78)) + iVar16;
        (*(int32_t*)rtguest::G((uint32_t)iStack_98)) = (*(int32_t*)rtguest::G((uint32_t)iStack_98)) + iVar16;
        iVar13 = iVar13 + iVar16;
      }
      if (0 < (*(int32_t*)rtguest::G((uint32_t)iStack_98))) {
        (*(uint32_t*)rtguest::G((uint32_t)uStack_6c)) = _lshift(param_1,(*(uint32_t*)rtguest::G((uint32_t)uStack_6c)),(*(int32_t*)rtguest::G((uint32_t)iStack_98)));
      }
      if (0 < (*(int32_t*)rtguest::G((uint32_t)iStack_78))) {
        (*(int32_t*)rtguest::G((uint32_t)iStack_60)) = _lshift(param_1,(*(int32_t*)rtguest::G((uint32_t)iStack_60)),(*(int32_t*)rtguest::G((uint32_t)iStack_78)));
      }
      if (((*(int32_t*)rtguest::G((uint32_t)iStack_80)) != 0) && (lVar4 = __mcmp((*(uint32_t*)rtguest::G((uint32_t)uStack_6c)),(*(int32_t*)rtguest::G((uint32_t)iStack_60))), lVar4 < 0)) {
        (*(uint32_t*)rtguest::G((uint32_t)uStack_6c)) = _multadd(param_1,(*(uint32_t*)rtguest::G((uint32_t)uStack_6c)),10,0);
        uVar22 = uVar22 - 1;
        if ((*(int32_t*)rtguest::G((uint32_t)iStack_7c)) != 0) {
          (*(int32_t*)rtguest::G((uint32_t)iStack_64)) = _multadd(param_1,(*(int32_t*)rtguest::G((uint32_t)iStack_64)),10,0);
        }
        (*(uint32_t*)rtguest::G((uint32_t)uStack_90)) = (*(uint32_t*)rtguest::G((uint32_t)uStack_88));
      }
      if (((int)(*(uint32_t*)rtguest::G((uint32_t)uStack_90)) < 1) && (2 < (int)(*(uint32_t*)rtguest::G((uint32_t)uStack_a8)))) {
        if ((int)(*(uint32_t*)rtguest::G((uint32_t)uStack_90)) < 0) goto LAB_002822b4;
        uVar5 = _multadd(param_1,(*(int32_t*)rtguest::G((uint32_t)iStack_60)),5,0);
        (*(int32_t*)rtguest::G((uint32_t)iStack_60)) = (int)uVar5;
        lVar4 = __mcmp((*(uint32_t*)rtguest::G((uint32_t)uStack_6c)),uVar5);
        uVar12 = uVar22;
        goto joined_r0x002822a8;
      }
      iVar16 = 1;
      pcVar20 = pcVar19;
      if ((*(int32_t*)rtguest::G((uint32_t)iStack_7c)) != 0) {
        if (0 < iVar13) {
          (*(int32_t*)rtguest::G((uint32_t)iStack_64)) = _lshift(param_1,(*(int32_t*)rtguest::G((uint32_t)iStack_64)),iVar13);
        }
        (*(int32_t*)rtguest::G((uint32_t)iStack_68)) = (*(int32_t*)rtguest::G((uint32_t)iStack_64));
        if (bVar2) {
          (*(int32_t*)rtguest::G((uint32_t)iStack_64)) = _Balloc(param_1,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_64)) + 4))));
          (uint32_t)(uintptr_t)(::memcpy((char *)rtguest::G((uint32_t)((uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_64)) + 0xc))), (char *)rtguest::G((uint32_t)((uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_68)) + 0xc))), (long)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_68)) + 0x10))) * 4 + 8)));
          (*(int32_t*)rtguest::G((uint32_t)iStack_64)) = _lshift(param_1,(*(int32_t*)rtguest::G((uint32_t)iStack_64)),1);
        }
        uVar12 = 1;
        do {
          iVar13 = quorem((*(uint32_t*)rtguest::G((uint32_t)uStack_6c)),(*(int32_t*)rtguest::G((uint32_t)iStack_60)));
          uVar17 = iVar13 + 0x30;
          lVar4 = __mcmp((*(uint32_t*)rtguest::G((uint32_t)uStack_6c)),(*(int32_t*)rtguest::G((uint32_t)iStack_68)));
          uVar5 = __mdiff(param_1,(*(int32_t*)rtguest::G((uint32_t)iStack_60)),(*(int32_t*)rtguest::G((uint32_t)iStack_64)));
          lVar14 = 1;
          if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar5 + 0xc))) == 0) {
            lVar14 = __mcmp((*(uint32_t*)rtguest::G((uint32_t)uStack_6c)),uVar5);
          }
          _Bfree(param_1,uVar5);
          cVar10 = (char)uVar17;
          if (((lVar14 == 0) && ((*(uint32_t*)rtguest::G((uint32_t)uStack_a8)) == 0)) && ((param_2 & 1) == 0)) {
            if (uVar17 != 0x39) {
              (*(int8_t*)rtguest::G((uint32_t)pcVar20)) = cVar10 + (0 < lVar4);
              goto LAB_00282620;
            }
LAB_002824f8:
            (*(int8_t*)rtguest::G((uint32_t)pcVar20)) = '9';
            pcVar20 = pcVar20 + 1;
            goto LAB_002825c8;
          }
          if ((lVar4 < 0) || (((lVar4 == 0 && ((*(uint32_t*)rtguest::G((uint32_t)uStack_a8)) == 0)) && ((param_2 & 1) == 0)))) {
            if (lVar14 < 1) {
              (*(int8_t*)rtguest::G((uint32_t)pcVar20)) = cVar10;
              goto LAB_00282620;
            }
            uVar5 = _lshift(param_1,(*(uint32_t*)rtguest::G((uint32_t)uStack_6c)),1);
            (*(uint32_t*)rtguest::G((uint32_t)uStack_6c)) = (uint32_t)uVar5;
            lVar4 = __mcmp(uVar5,(*(int32_t*)rtguest::G((uint32_t)iStack_60)));
            if (lVar4 < 1) {
              if (lVar4 != 0) {
                (*(int8_t*)rtguest::G((uint32_t)pcVar20)) = cVar10;
                goto LAB_00282620;
              }
              if ((uVar17 & 1) == 0) {
                (*(int8_t*)rtguest::G((uint32_t)pcVar20)) = cVar10;
                goto LAB_00282620;
              }
            }
            if (iVar13 + 0x31 == 0x3a) goto LAB_002824f8;
            (*(int8_t*)rtguest::G((uint32_t)pcVar20)) = (char)(iVar13 + 0x31);
            goto LAB_00282620;
          }
          if (0 < lVar14) {
            if (uVar17 == 0x39) goto LAB_002824f8;
            (*(int8_t*)rtguest::G((uint32_t)pcVar20)) = cVar10 + '\x01';
            goto LAB_00282620;
          }
          (*(int8_t*)rtguest::G((uint32_t)pcVar20)) = cVar10;
          pcVar20 = pcVar20 + 1;
          if (uVar12 == (*(uint32_t*)rtguest::G((uint32_t)uStack_90))) goto LAB_0028256c;
          (*(uint32_t*)rtguest::G((uint32_t)uStack_6c)) = _multadd(param_1,(*(uint32_t*)rtguest::G((uint32_t)uStack_6c)),10,0);
          if ((*(int32_t*)rtguest::G((uint32_t)iStack_68)) == (*(int32_t*)rtguest::G((uint32_t)iStack_64))) {
            (*(int32_t*)rtguest::G((uint32_t)iStack_68)) = _multadd(param_1,(*(int32_t*)rtguest::G((uint32_t)iStack_64)),10,0);
            (*(int32_t*)rtguest::G((uint32_t)iStack_64)) = (*(int32_t*)rtguest::G((uint32_t)iStack_68));
          }
          else {
            (*(int32_t*)rtguest::G((uint32_t)iStack_68)) = _multadd(param_1,(*(int32_t*)rtguest::G((uint32_t)iStack_68)),10,0);
            (*(int32_t*)rtguest::G((uint32_t)iStack_64)) = _multadd(param_1,(*(int32_t*)rtguest::G((uint32_t)iStack_64)),10,0);
          }
          uVar12 = uVar12 + 1;
        } while( true );
      }
      while( true ) {
        iVar13 = quorem((*(uint32_t*)rtguest::G((uint32_t)uStack_6c)),(*(int32_t*)rtguest::G((uint32_t)iStack_60)));
        uVar17 = iVar13 + 0x30;
        (*(int8_t*)rtguest::G((uint32_t)pcVar20)) = (char)uVar17;
        pcVar20 = pcVar20 + 1;
        if ((int)(*(uint32_t*)rtguest::G((uint32_t)uStack_90)) <= iVar16) break;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_6c)) = _multadd(param_1,(*(uint32_t*)rtguest::G((uint32_t)uStack_6c)),10,0);
        iVar16 = iVar16 + 1;
      }
LAB_0028256c:
      uVar5 = _lshift(param_1,(*(uint32_t*)rtguest::G((uint32_t)uStack_6c)),1);
      (*(uint32_t*)rtguest::G((uint32_t)uStack_6c)) = (uint32_t)uVar5;
      lVar4 = __mcmp(uVar5,(*(int32_t*)rtguest::G((uint32_t)iStack_60)));
      if (lVar4 < 1) {
        if (lVar4 == 0) {
          if ((uVar17 & 1) != 0) goto LAB_002825c8;
          pcVar20 = pcVar20 + -1;
        }
        else {
          pcVar20 = pcVar20 + -1;
        }
        for (; (*(int8_t*)rtguest::G((uint32_t)pcVar20)) == '0'; pcVar20 = pcVar20 + -1) {
        }
LAB_00282620:
        pcVar20 = pcVar20 + 1;
      }
      else {
LAB_002825c8:
        do {
          pcVar20 = pcVar20 + -1;
          if ((*(int8_t*)rtguest::G((uint32_t)pcVar20)) != '9') {
            (*(int8_t*)rtguest::G((uint32_t)pcVar20)) = (*(int8_t*)rtguest::G((uint32_t)pcVar20)) + '\x01';
            goto LAB_00282620;
          }
        } while (pcVar20 != pcVar19);
        uVar22 = uVar22 + 1;
        (*(int8_t*)rtguest::G((uint32_t)pcVar19)) = '1';
        pcVar20 = pcVar19 + 1;
      }
    }
    else {
      uVar5 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x00332430 + uVar22 * 8)));
      uVar17 = 1;
      pcVar20 = pcVar19;
      uVar12 = uVar22;
      if ((-1 < (int)(*(uint32_t*)rtguest::G((uint32_t)uStack_a4))) || (0 < (int)(*(uint32_t*)rtguest::G((uint32_t)uStack_90)))) {
        do {
          uVar6 = dpdiv(param_2,uVar5);
          uVar11 = dptoli(uVar6);
          uVar6 = litodp(uVar11);
          uVar6 = dpmul(uVar6,uVar5);
          uVar6 = dpsub(param_2,uVar6);
          (*(int8_t*)rtguest::G((uint32_t)pcVar20)) = (char)uVar11 + '0';
          pcVar20 = pcVar20 + 1;
          if (uVar17 == (*(uint32_t*)rtguest::G((uint32_t)uStack_90))) {
            uVar6 = dpadd(uVar6);
            lVar4 = dpcmp(uVar6,uVar5);
            pcVar21 = pcVar20;
            if ((0 < lVar4) || ((lVar4 = dpcmp(uVar6,uVar5), lVar4 == 0 && ((uVar11 & 1) != 0))))
            goto LAB_00281eb8;
            break;
          }
          param_2 = dpmul(0x4024000000000000);
          lVar4 = dpcmp(param_2,0);
          uVar17 = uVar17 + 1;
        } while (lVar4 != 0);
        goto LAB_00282664;
      }
      (*(int32_t*)rtguest::G((uint32_t)iStack_64)) = 0;
      (*(int32_t*)rtguest::G((uint32_t)iStack_60)) = 0;
      if ((int)(*(uint32_t*)rtguest::G((uint32_t)uStack_90)) < 0) goto LAB_002822b4;
      uVar5 = dpmul(uVar5,0x4014000000000000);
      lVar4 = dpcmp(param_2,uVar5);
joined_r0x002822a8:
      if (lVar4 < 1) goto LAB_002822b4;
LAB_002822c0:
      uVar22 = uVar12 + 1;
      (*(int8_t*)rtguest::G((uint32_t)pcVar19)) = '1';
      pcVar20 = pcVar19 + 1;
    }
  }
  else {
    (*(uint32_t*)rtguest::G((uint32_t)uStack_8c)) = (*(uint32_t*)rtguest::G((uint32_t)uStack_90));
    iVar13 = 2;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_84)) = uVar12;
    if ((int)uVar12 < 1) {
      uVar17 = -uVar12;
      uVar11 = param_2;
      if (uVar17 != 0) {
        uVar22 = (int)uVar17 >> 4;
        uVar11 = dpmul((*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x00332430 + (uVar17 & 0xf) * 8))),param_2);
        if (uVar22 != 0) {
          puVar15 = (uint32_t)0x003324f8;
          do {
            uVar17 = uVar22 & 1;
            uVar22 = (int)uVar22 >> 1;
            if (uVar17 != 0) {
              iVar13 = iVar13 + 1;
              uVar11 = dpmul((*(uint64_t*)rtguest::G((uint32_t)puVar15)),uVar11);
            }
            puVar15 = puVar15 + 1;
          } while (uVar22 != 0);
        }
      }
    }
    else {
      uVar17 = (int)uVar12 >> 4;
      uVar5 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x00332430 + (uVar12 & 0xf) * 8)));
      uVar11 = param_2;
      if ((uVar17 & 0x10) != 0) {
        uVar17 = uVar17 & 0xf;
        iVar13 = 3;
        uVar11 = dpdiv(param_2,0x75154fdd7f73bf3c);
      }
      if (uVar17 != 0) {
        puVar15 = (uint32_t)0x003324f8;
        do {
          uVar22 = uVar17 & 1;
          uVar17 = (int)uVar17 >> 1;
          if (uVar22 != 0) {
            iVar13 = iVar13 + 1;
            uVar5 = dpmul(uVar5,(*(uint64_t*)rtguest::G((uint32_t)puVar15)));
          }
          puVar15 = puVar15 + 1;
        } while (uVar17 != 0);
      }
      uVar11 = dpdiv(uVar11,uVar5);
    }
    if ((((*(int32_t*)rtguest::G((uint32_t)iStack_80)) != 0) && (lVar4 = dpcmp(uVar11,0x3ff0000000000000), lVar4 < 0)) &&
       (0 < (int)(*(uint32_t*)rtguest::G((uint32_t)uStack_90)))) {
      uVar17 = (*(uint32_t*)rtguest::G((uint32_t)uStack_8c));
      uVar22 = (*(uint32_t*)rtguest::G((uint32_t)uStack_84));
      if ((int)(*(uint32_t*)rtguest::G((uint32_t)uStack_88)) < 1) goto LAB_00281d84;
      uVar12 = uVar12 - 1;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_90)) = (*(uint32_t*)rtguest::G((uint32_t)uStack_88));
      iVar13 = iVar13 + 1;
      uVar11 = dpmul(0x4024000000000000,uVar11);
    }
    uVar5 = litodp(iVar13);
    uVar5 = dpmul(uVar5,uVar11);
    uVar7 = dpadd(uVar5,0x401c000000000000);
    uVar7 = uVar7 & 0xffffffff | (long)((int)(uVar7 >> 0x20) + -0x3400000) << 0x20;
    if ((*(uint32_t*)rtguest::G((uint32_t)uStack_90)) != 0) {
      if ((*(int32_t*)rtguest::G((uint32_t)iStack_7c)) != 0) {
        iVar13 = 0;
        uVar5 = dpdiv(0x3fe0000000000000,(*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x00332428 + (*(uint32_t*)rtguest::G((uint32_t)uStack_90)) * 8))));
        uVar5 = dpsub(uVar5,uVar7);
        pcVar20 = pcVar19;
        while( true ) {
          uVar6 = dptoli(uVar11);
          uVar8 = litodp(uVar6);
          uVar8 = dpsub(uVar11,uVar8);
          (*(int8_t*)rtguest::G((uint32_t)pcVar20)) = (char)uVar6 + '0';
          pcVar20 = pcVar20 + 1;
          lVar4 = dpcmp(uVar8,uVar5);
          if (lVar4 < 0) break;
          uVar6 = dpsub(0x3ff0000000000000,uVar8);
          lVar4 = dpcmp(uVar6,uVar5);
          pcVar21 = pcVar20;
          if (lVar4 < 0) goto LAB_00281eb8;
          iVar13 = iVar13 + 1;
          uVar17 = (*(uint32_t*)rtguest::G((uint32_t)uStack_8c));
          uVar22 = (*(uint32_t*)rtguest::G((uint32_t)uStack_84));
          if ((int)(*(uint32_t*)rtguest::G((uint32_t)uStack_90)) <= iVar13) goto LAB_00281d84;
          uVar5 = dpmul(0x4024000000000000,uVar5);
          uVar11 = dpmul(0x4024000000000000,uVar8);
        }
        goto LAB_00282664;
      }
      uVar17 = 1;
      uVar5 = dpmul((*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x00332428 + (*(uint32_t*)rtguest::G((uint32_t)uStack_90)) * 8))),uVar7);
      pcVar20 = pcVar19;
      while( true ) {
        uVar6 = dptoli(uVar11);
        uVar8 = litodp(uVar6);
        uVar8 = dpsub(uVar11,uVar8);
        (*(int8_t*)rtguest::G((uint32_t)pcVar20)) = (char)uVar6 + '0';
        if (uVar17 == (*(uint32_t*)rtguest::G((uint32_t)uStack_90))) break;
        uVar17 = uVar17 + 1;
        uVar11 = dpmul(0x4024000000000000,uVar8);
        pcVar20 = pcVar20 + 1;
      }
      uVar6 = dpadd(0x3fe0000000000000,uVar5);
      lVar4 = dpcmp(uVar8,uVar6);
      pcVar21 = pcVar20 + 1;
      if (0 < lVar4) {
LAB_00281eb8:
        do {
          pcVar20 = pcVar21;
          pcVar21 = pcVar20 + -1;
          cVar10 = (*(int8_t*)rtguest::G((uint32_t)pcVar21));
          if ((*(int8_t*)rtguest::G((uint32_t)pcVar21)) != '9') goto LAB_00281ee8;
        } while (pcVar21 != pcVar19);
        (*(int8_t*)rtguest::G((uint32_t)pcVar19)) = '0';
        cVar10 = '0';
        uVar12 = uVar12 + 1;
LAB_00281ee8:
        (*(int8_t*)rtguest::G((uint32_t)pcVar21)) = cVar10 + '\x01';
        goto LAB_00282664;
      }
      uVar5 = dpsub(0x3fe0000000000000,uVar5);
      lVar4 = dpcmp(uVar8,uVar5);
      uVar17 = (*(uint32_t*)rtguest::G((uint32_t)uStack_8c));
      uVar22 = (*(uint32_t*)rtguest::G((uint32_t)uStack_84));
      if (lVar4 < 0) {
        for (; (*(int8_t*)rtguest::G((uint32_t)pcVar20)) == '0'; pcVar20 = pcVar20 + -1) {
        }
        pcVar20 = pcVar20 + 1;
        goto LAB_00282664;
      }
      goto LAB_00281d84;
    }
    (*(int32_t*)rtguest::G((uint32_t)iStack_64)) = 0;
    (*(int32_t*)rtguest::G((uint32_t)iStack_60)) = 0;
    uVar5 = dpsub(uVar11,0x4014000000000000);
    lVar4 = dpcmp(uVar5,uVar7);
    if (0 < lVar4) goto LAB_002822c0;
    uVar6 = dpsub(0,uVar7);
    lVar4 = dpcmp(uVar5,uVar6);
    uVar17 = (*(uint32_t*)rtguest::G((uint32_t)uStack_8c));
    uVar22 = (*(uint32_t*)rtguest::G((uint32_t)uStack_84));
    if (-1 < lVar4) goto LAB_00281d84;
LAB_002822b4:
    uVar22 = ~(*(uint32_t*)rtguest::G((uint32_t)uStack_a4));
    pcVar20 = pcVar19;
  }
  _Bfree(param_1,(*(int32_t*)rtguest::G((uint32_t)iStack_60)));
  uVar12 = uVar22;
  if ((*(int32_t*)rtguest::G((uint32_t)iStack_64)) != 0) {
    if (((*(int32_t*)rtguest::G((uint32_t)iStack_68)) != 0) && ((*(int32_t*)rtguest::G((uint32_t)iStack_68)) != (*(int32_t*)rtguest::G((uint32_t)iStack_64)))) {
      _Bfree(param_1,(*(int32_t*)rtguest::G((uint32_t)iStack_68)));
    }
    _Bfree(param_1,(*(int32_t*)rtguest::G((uint32_t)iStack_64)));
  }
LAB_00282664:
  _Bfree(param_1,(*(uint32_t*)rtguest::G((uint32_t)uStack_6c)));
  (*(int8_t*)rtguest::G((uint32_t)pcVar20)) = '\0';
  (*(int32_t*)rtguest::G((uint32_t)piStack_a0)) = uVar12 + 1;
  if (puStack_9c != (uint32_t)0x0) {
    (*(uint32_t*)rtguest::G((uint32_t)puStack_9c)) = pcVar20;
  }
  return pcVar19;
}


RT_GUEST_FN(0x00281458, guest::_dtoa_r)

}  // namespace guest
