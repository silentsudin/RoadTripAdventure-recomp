// guest:0x0028df38 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __udivdi3(uint32_t param_1, uint32_t param_2, ...) {
  long lVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  int iVar11;
  uint uVar12;
  uint uVar13;
  ulong uVar14;
  uint uVar15;
  ulong uVar16;
  ulong uVar17;
  uint uVar18;
  ulong uVar19;
  ulong uVar20;
  
  uVar14 = param_2 >> 0x20;
  uVar17 = param_1 >> 0x20;
  uVar15 = (uint32_t)param_2;
  uVar16 = (uint32_t)(int)uVar15;
  uVar18 = (uint32_t)param_1;
  uVar19 = (uint32_t)(int)uVar18;
  uVar9 = (uint32_t)((uint32_t)param_1 >> 0x20);
  uVar10 = (uint32_t)((uint32_t)param_2 >> 0x20);
  if (uVar14 == 0) {
    if (uVar16 <= uVar17) {
      if (uVar16 == 0) {
        trap(7);
        uVar16 = (uint32_t)(1 / (int)uVar10);
      }
      if (uVar16 < 0x10000) {
        iVar7 = 8;
        if (uVar16 < 0x100) {
          iVar7 = 0;
        }
      }
      else {
        iVar7 = 0x10;
        if (0xffffff < uVar16) {
          iVar7 = 0x18;
        }
      }
      uVar15 = (uint32_t)uVar16;
      uVar10 = 0x20 - ((uint32_t)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x3329f0 + (uint32_t)(uVar15 >> iVar7) * 4)) + iVar7);
      if (uVar10 == 0) {
        iVar5 = uVar9 - uVar15;
        uVar14 = 1;
        uVar10 = uVar15 >> 0x10;
        uVar15 = uVar15 & 0xffff;
      }
      else {
        uVar19 = (uint32_t)(int)(uVar18 << (uVar10 & 0x1f));
        uVar8 = uVar9 >> (0x20 - uVar10 & 0x1f);
        uVar9 = uVar9 << (uVar10 & 0x1f) | uVar18 >> (0x20 - uVar10 & 0x1f);
        uVar18 = uVar15 << (uVar10 & 0x1f);
        uVar16 = (uint32_t)(int)uVar18;
        uVar10 = uVar18 >> 0x10;
        iVar7 = (int)uVar8 / (int)uVar10;
        uVar15 = uVar18 & 0xffff;
        if (uVar10 == 0) {
          trap(7);
        }
        iVar4 = iVar7 * uVar15;
        uVar8 = (int)uVar8 % (int)uVar10 << 0x10 | uVar9 >> 0x10;
        iVar11 = iVar7;
        if ((uint32_t)(long)(int)uVar8 < (uint32_t)(long)iVar4) {
          uVar8 = uVar8 + uVar18;
          iVar11 = iVar7 + -1;
          if ((uint32_t)(long)(int)uVar8 < uVar16) goto LAB_0028e1a8;
          if ((uint32_t)(long)(int)uVar8 < (uint32_t)(long)iVar4) {
            iVar11 = iVar7 + -2;
            uVar8 = uVar8 + uVar18;
            goto LAB_0028e1a8;
          }
          iVar4 = uVar8 - iVar4;
        }
        else {
LAB_0028e1a8:
          iVar4 = uVar8 - iVar4;
        }
        if (uVar10 == 0) {
          trap(7);
        }
        iVar7 = iVar4 / (int)uVar10;
        uVar20 = (uint32_t)iVar7;
        iVar5 = iVar7 * uVar15;
        uVar17 = (long)(iVar4 % (int)uVar10 << 0x10) | (long)(int)uVar9 & 0xffffU;
        uVar14 = (uint32_t)(iVar11 << 0x10);
        if (uVar17 < (uint32_t)(long)iVar5) {
          iVar4 = (int)uVar17 + uVar18;
          uVar17 = (uint32_t)iVar4;
          uVar20 = (uint32_t)(iVar7 + -1);
          if (uVar16 <= uVar17) {
            uVar14 = (uint32_t)(iVar11 << 0x10);
            if ((uint32_t)(long)iVar5 <= uVar17) goto LAB_0028e208;
            uVar20 = (uint32_t)(iVar7 + -2);
            uVar17 = (uint32_t)(int)(iVar4 + uVar18);
          }
          uVar14 = (uint32_t)(iVar11 << 0x10);
        }
LAB_0028e208:
        iVar5 = (int)uVar17 - iVar5;
        uVar14 = uVar14 | uVar20;
      }
      iVar7 = iVar5 / (int)uVar10;
      if (uVar10 == 0) {
        trap(7);
      }
      iVar4 = iVar7 * uVar15;
      uVar9 = iVar5 % (int)uVar10 << 0x10 | (uint32_t)uVar19 >> 0x10;
      iVar11 = (int)uVar16;
      if ((uint32_t)(long)(int)uVar9 < (uint32_t)(long)iVar4) {
        iVar6 = uVar9 + iVar11;
        iVar5 = iVar7 + -1;
        if (uVar16 <= (uint32_t)(long)iVar6) {
          if ((uint32_t)(long)iVar4 <= (uint32_t)(long)iVar6) {
            iVar4 = iVar6 - iVar4;
            goto LAB_0028e274;
          }
          iVar5 = iVar7 + -2;
          iVar6 = iVar6 + iVar11;
        }
        iVar4 = iVar6 - iVar4;
      }
      else {
        iVar4 = uVar9 - iVar4;
        iVar5 = iVar7;
      }
LAB_0028e274:
      if (uVar10 == 0) {
        trap(7);
      }
      uVar9 = iVar4 / (int)uVar10;
      uVar17 = (long)(iVar4 % (int)uVar10 << 0x10) | uVar19 & 0xffff;
      uVar10 = uVar9;
      if (uVar17 < (uint32_t)(long)(int)(uVar9 * uVar15)) {
        uVar17 = (uint32_t)((int)uVar17 + iVar11);
        uVar10 = uVar9 - 1;
        if ((uVar16 <= uVar17) && (uVar17 < (uint32_t)(long)(int)(uVar9 * uVar15))) {
          uVar10 = uVar9 - 2;
        }
      }
      uVar10 = iVar5 << 0x10 | uVar10;
      goto LAB_0028e484;
    }
    if (uVar16 < 0x10000) {
      iVar7 = 8;
      if (uVar16 < 0x100) {
        iVar7 = 0;
      }
    }
    else {
      iVar7 = 0x10;
      if (0xffffff < uVar16) {
        iVar7 = 0x18;
      }
    }
    uVar10 = 0x20 - ((uint32_t)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x3329f0 + (uint32_t)(uVar15 >> iVar7) * 4)) + iVar7);
    if (uVar10 != 0) {
      uVar19 = (uint32_t)(int)(uVar18 << (uVar10 & 0x1f));
      uVar17 = (uint32_t)(int)(uVar9 << (uVar10 & 0x1f) | uVar18 >> (0x20 - uVar10 & 0x1f));
      uVar16 = (uint32_t)(int)(uVar15 << (uVar10 & 0x1f));
    }
    uVar15 = (uint32_t)uVar16;
    uVar10 = uVar15 >> 0x10;
    iVar7 = (int)uVar17 / (int)uVar10;
    if (uVar10 == 0) {
      trap(7);
    }
    iVar4 = iVar7 * (uVar15 & 0xffff);
    uVar9 = (int)uVar17 % (int)uVar10 << 0x10 | (uint32_t)uVar19 >> 0x10;
    if ((uint32_t)(long)(int)uVar9 < (uint32_t)(long)iVar4) {
      iVar5 = uVar9 + uVar15;
      iVar11 = iVar7 + -1;
      if (uVar16 <= (uint32_t)(long)iVar5) {
        if ((uint32_t)(long)iVar4 <= (uint32_t)(long)iVar5) {
          iVar4 = iVar5 - iVar4;
          goto LAB_0028e034;
        }
        iVar11 = iVar7 + -2;
        iVar5 = iVar5 + uVar15;
      }
      iVar4 = iVar5 - iVar4;
    }
    else {
      iVar4 = uVar9 - iVar4;
      iVar11 = iVar7;
    }
LAB_0028e034:
    if (uVar10 == 0) {
      trap(7);
    }
    uVar9 = iVar4 / (int)uVar10;
    uVar14 = (uint32_t)(int)(uVar9 * (uVar15 & 0xffff));
    uVar17 = (long)(iVar4 % (int)uVar10 << 0x10) | uVar19 & 0xffff;
    uVar10 = uVar9;
    if (uVar17 < uVar14) {
      uVar17 = (uint32_t)(int)((int)uVar17 + uVar15);
      uVar10 = uVar9 - 1;
      if ((uVar16 <= uVar17) && (uVar17 < uVar14)) {
        uVar10 = uVar9 - 2;
      }
    }
    uVar10 = iVar11 << 0x10 | uVar10;
  }
  else if (uVar17 < uVar14) {
    uVar10 = 0;
  }
  else {
    if (uVar14 < 0x10000) {
      iVar7 = 8;
      if (uVar14 < 0x100) {
        iVar7 = 0;
      }
    }
    else {
      iVar7 = 0x10;
      if (0xffffff < uVar14) {
        iVar7 = 0x18;
      }
    }
    uVar8 = 0x20 - ((uint32_t)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x3329f0 + (uint32_t)(uVar10 >> iVar7) * 4)) + iVar7);
    uVar12 = 0x20 - uVar8;
    if (uVar8 == 0) {
      uVar10 = 1;
      if ((uVar17 <= uVar14) && (uVar10 = 0, uVar16 <= uVar19)) {
        uVar10 = 1;
      }
    }
    else {
      uVar13 = uVar10 << (uVar8 & 0x1f) | uVar15 >> (uVar12 & 0x1f);
      uVar10 = uVar9 >> (uVar12 & 0x1f);
      uVar12 = uVar9 << (uVar8 & 0x1f) | uVar18 >> (uVar12 & 0x1f);
      uVar9 = uVar13 >> 0x10;
      iVar7 = (int)uVar10 / (int)uVar9;
      if (uVar9 == 0) {
        trap(7);
      }
      uVar2 = iVar7 * (uVar13 & 0xffff);
      uVar10 = (int)uVar10 % (int)uVar9 << 0x10 | uVar12 >> 0x10;
      if (uVar10 < uVar2) {
        uVar10 = uVar10 + uVar13;
        iVar4 = iVar7 + -1;
        if (uVar13 <= uVar10) {
          if (uVar2 <= uVar10) {
            iVar11 = uVar10 - uVar2;
            goto LAB_0028e3ec;
          }
          iVar4 = iVar7 + -2;
          uVar10 = uVar10 + uVar13;
        }
        iVar11 = uVar10 - uVar2;
      }
      else {
        iVar11 = uVar10 - uVar2;
        iVar4 = iVar7;
      }
LAB_0028e3ec:
      if (uVar9 == 0) {
        trap(7);
      }
      uVar2 = iVar11 / (int)uVar9;
      uVar3 = uVar2 * (uVar13 & 0xffff);
      uVar9 = iVar11 % (int)uVar9 << 0x10 | uVar12 & 0xffff;
      uVar10 = uVar2;
      if (uVar9 < uVar3) {
        uVar9 = uVar9 + uVar13;
        uVar10 = uVar2 - 1;
        if ((uVar13 <= uVar9) && (uVar9 < uVar3)) {
          uVar9 = uVar9 + uVar13;
          uVar10 = uVar2 - 2;
        }
      }
      uVar10 = iVar4 << 0x10 | uVar10;
      lVar1 = (uint32_t)uVar10 * (uint32_t)(uVar15 << (uVar8 & 0x1f));
      uVar15 = (uint32_t)((uint32_t)lVar1 >> 0x20);
      if (uVar9 - uVar3 < uVar15) {
        uVar10 = uVar10 - 1;
      }
      else {
        uVar14 = 0;
        if ((uVar15 != uVar9 - uVar3) || ((uint32_t)lVar1 <= uVar18 << (uVar8 & 0x1f)))
        uVar10 = uVar10 - 1;
      }
    }
  }
  uVar14 = 0;
LAB_0028e484:
  return (uint32_t)uVar10 | uVar14 << 0x20;
}


RT_GUEST_FN(0x0028df38, guest::__udivdi3)

}  // namespace guest
