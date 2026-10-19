// guest:0x0028d8a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __moddi3(uint32_t param_1, uint32_t param_2, ...) {
    uint32_t uStack_20 = rtguest::allocFrame(4);
  long lVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  ulong uVar8;
  uint uVar9;
  ulong uVar10;
  uint uVar11;
  ulong uVar12;
  uint uVar13;
  uint uVar14;
  uint uVar15;
  ulong uVar16;
  int iVar17;
  int iVar18;
  
  iVar18 = 0;
  if ((long)param_1 < 0) {
    iVar18 = -1;
    param_1 = CONCAT44(-(uint32_t)(-(int)param_1 != 0) - (int)(param_1 >> 0x20),-(int)param_1);
  }
  if (param_2 >> 0x20 < 0) {
    param_2 = CONCAT44(-(uint32_t)(-(int)param_2 != 0) - (int)((uint32_t)param_2 >> 0x20),-(int)param_2);
  }
  uVar10 = param_2 >> 0x20;
  uVar14 = (uint32_t)param_1;
  uVar16 = (uint32_t)(int)uVar14;
  uVar11 = (uint32_t)(param_1 >> 0x20);
  uVar12 = (uint32_t)(int)uVar11;
  uVar6 = (uint32_t)param_2;
  uVar8 = (uint32_t)(int)uVar6;
  uVar13 = (uint32_t)((uint32_t)param_2 >> 0x20);
  if (uVar10 != 0) {
    if (uVar12 < uVar10) {
      (*(uint32_t*)rtguest::G((uint32_t)uStack_20)) = param_1 & 0xffffffff | uVar12 << 0x20;
      goto LAB_0028decc;
    }
    if (uVar10 < 0x10000) {
      iVar4 = 8;
      if (uVar10 < 0x100) {
        iVar4 = 0;
      }
    }
    else {
      iVar4 = 0x10;
      if (0xffffff < uVar10) {
        iVar4 = 0x18;
      }
    }
    uVar3 = 0x20 - ((uint32_t)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x3328f0 + (uint32_t)(uVar13 >> iVar4) * 4)) + iVar4);
    uVar7 = 0x20 - uVar3;
    if (uVar3 == 0) {
      if ((uVar10 < uVar12) || (uVar10 = uVar16, uVar8 <= uVar16)) {
        uVar10 = (uint32_t)(int)(uVar14 - uVar6);
        uVar12 = (uint32_t)(int)((uVar11 - uVar13) - (uint32_t)(uVar16 < uVar10));
      }
      if (0x00000000u != (uint32_t)0x20) {
        (*(uint32_t*)rtguest::G((uint32_t)uStack_20)) = uVar10 & 0xffffffff | uVar12 << 0x20;
      }
    }
    uVar15 = uVar14 << (uVar3 & 0x1f);
    uVar9 = uVar13 << (uVar3 & 0x1f) | uVar6 >> (uVar7 & 0x1f);
    uVar13 = uVar11 >> (uVar7 & 0x1f);
    uVar6 = uVar6 << (uVar3 & 0x1f);
    uVar14 = uVar11 << (uVar3 & 0x1f) | uVar14 >> (uVar7 & 0x1f);
    uVar11 = uVar9 >> 0x10;
    iVar4 = (int)uVar13 / (int)uVar11;
    if (uVar11 == 0) {
      trap(7);
    }
    uVar2 = iVar4 * (uVar9 & 0xffff);
    uVar13 = (int)uVar13 % (int)uVar11 << 0x10 | uVar14 >> 0x10;
    if (uVar13 < uVar2) {
      uVar13 = uVar13 + uVar9;
      iVar17 = iVar4 + -1;
      if (uVar9 <= uVar13) {
        if (uVar2 <= uVar13) {
          iVar5 = uVar13 - uVar2;
          goto LAB_0028ddd4;
        }
        iVar17 = iVar4 + -2;
        uVar13 = uVar13 + uVar9;
      }
      iVar5 = uVar13 - uVar2;
    }
    else {
      iVar5 = uVar13 - uVar2;
      iVar17 = iVar4;
    }
LAB_0028ddd4:
    if (uVar11 == 0) {
      trap(7);
    }
    uVar13 = iVar5 / (int)uVar11;
    uVar2 = uVar13 * (uVar9 & 0xffff);
    uVar11 = iVar5 % (int)uVar11 << 0x10 | uVar14 & 0xffff;
    if (uVar11 < uVar2) {
      uVar11 = uVar11 + uVar9;
      uVar14 = uVar13 - 1;
      if (uVar9 <= uVar11) {
        if (uVar2 <= uVar11) {
          uVar11 = uVar11 - uVar2;
          goto LAB_0028de30;
        }
        uVar14 = uVar13 - 2;
        uVar11 = uVar11 + uVar9;
      }
      uVar11 = uVar11 - uVar2;
    }
    else {
      uVar11 = uVar11 - uVar2;
      uVar14 = uVar13;
    }
LAB_0028de30:
    lVar1 = (uint32_t)(iVar17 << 0x10 | uVar14) * (uint32_t)uVar6;
    uVar14 = (uint32_t)lVar1;
    uVar13 = (uint32_t)((uint32_t)lVar1 >> 0x20);
    if ((uVar11 < uVar13) || ((uVar13 == uVar11 && (uVar15 < uVar14)))) {
      uVar6 = uVar14 - uVar6;
      lVar1 = CONCAT44((uVar13 - uVar9) - (uint32_t)(uVar14 < uVar6),uVar6);
    }
    uVar6 = uVar15 - (int)lVar1;
    if (0x00000000u != (uint32_t)0x20) {
      uVar11 = (uVar11 - (int)((uint32_t)lVar1 >> 0x20)) - (uint32_t)(uVar15 < uVar6);
      (*(uint32_t*)rtguest::G((uint32_t)uStack_20)) = CONCAT44(uVar11 >> (uVar3 & 0x1f),
                           uVar11 << (uVar7 & 0x1f) | uVar6 >> (uVar3 & 0x1f));
    }
  }
  if (uVar12 < uVar8) {
    if (uVar8 < 0x10000) {
      iVar4 = 8;
      if (uVar8 < 0x100) {
        iVar4 = 0;
      }
    }
    else {
      iVar4 = 0x10;
      if (0xffffff < uVar8) {
        iVar4 = 0x18;
      }
    }
    uVar13 = 0x20 - ((uint32_t)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x3328f0 + (uint32_t)(uVar6 >> iVar4) * 4)) + iVar4);
    if (uVar13 != 0) {
      uVar16 = (uint32_t)(int)(uVar14 << (uVar13 & 0x1f));
      uVar11 = uVar11 << (uVar13 & 0x1f) | uVar14 >> (0x20 - uVar13 & 0x1f);
      uVar8 = (uint32_t)(int)(uVar6 << (uVar13 & 0x1f));
    }
    uVar14 = (uint32_t)uVar8 >> 0x10;
    uVar6 = (uint32_t)uVar8 & 0xffff;
  }
  else {
    if (uVar8 == 0) {
      trap(7);
      uVar8 = (uint32_t)(1 / (int)uVar13);
    }
    if (uVar8 < 0x10000) {
      iVar4 = 8;
      if (uVar8 < 0x100) {
        iVar4 = 0;
      }
    }
    else {
      iVar4 = 0x10;
      if (0xffffff < uVar8) {
        iVar4 = 0x18;
      }
    }
    uVar6 = (uint32_t)uVar8;
    uVar13 = 0x20 - ((uint32_t)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x3328f0 + (uint32_t)(uVar6 >> iVar4) * 4)) + iVar4);
    if (uVar13 == 0) {
      uVar11 = uVar11 - uVar6;
      uVar14 = uVar6 >> 0x10;
      uVar6 = uVar6 & 0xffff;
    }
    else {
      uVar16 = (uint32_t)(int)(uVar14 << (uVar13 & 0x1f));
      uVar3 = uVar11 >> (0x20 - uVar13 & 0x1f);
      uVar11 = uVar11 << (uVar13 & 0x1f) | uVar14 >> (0x20 - uVar13 & 0x1f);
      uVar7 = uVar6 << (uVar13 & 0x1f);
      uVar8 = (uint32_t)(int)uVar7;
      uVar14 = uVar7 >> 0x10;
      uVar6 = uVar7 & 0xffff;
      if (uVar14 == 0) {
        trap(7);
      }
      iVar4 = ((int)uVar3 / (int)uVar14) * uVar6;
      uVar3 = (int)uVar3 % (int)uVar14 << 0x10 | uVar11 >> 0x10;
      if ((uint32_t)(long)(int)uVar3 < (uint32_t)(long)iVar4) {
        uVar3 = uVar3 + uVar7;
        if (uVar8 <= (uint32_t)(long)(int)uVar3) {
          if ((uint32_t)(long)(int)uVar3 < (uint32_t)(long)iVar4) {
            uVar3 = uVar3 + uVar7;
          }
          goto LAB_0028db18;
        }
        iVar4 = uVar3 - iVar4;
      }
      else {
LAB_0028db18:
        iVar4 = uVar3 - iVar4;
      }
      if (uVar14 == 0) {
        trap(7);
      }
      iVar17 = (iVar4 / (int)uVar14) * uVar6;
      uVar10 = (long)(iVar4 % (int)uVar14 << 0x10) | (long)(int)uVar11 & 0xffffU;
      iVar4 = (int)uVar10;
      uVar11 = iVar4 - iVar17;
      if (uVar10 < (uint32_t)(long)iVar17) {
        iVar4 = iVar4 + uVar7;
        uVar11 = iVar4 - iVar17;
        if (uVar8 <= (uint32_t)(long)iVar4) {
          if ((uint32_t)(long)iVar4 < (uint32_t)(long)iVar17) {
            iVar4 = iVar4 + uVar7;
          }
          uVar11 = iVar4 - iVar17;
        }
      }
    }
  }
  if (uVar14 == 0) {
    trap(7);
  }
  iVar4 = ((int)uVar11 / (int)uVar14) * uVar6;
  uVar11 = (int)uVar11 % (int)uVar14 << 0x10 | (uint32_t)uVar16 >> 0x10;
  iVar17 = (int)uVar8;
  if ((uint32_t)(long)(int)uVar11 < (uint32_t)(long)iVar4) {
    iVar5 = uVar11 + iVar17;
    if ((uint32_t)(long)iVar5 < uVar8) {
      iVar4 = iVar5 - iVar4;
    }
    else {
      if ((uint32_t)(long)iVar5 < (uint32_t)(long)iVar4) {
        iVar5 = iVar5 + iVar17;
      }
      iVar4 = iVar5 - iVar4;
    }
  }
  else {
    iVar4 = uVar11 - iVar4;
  }
  if (uVar14 == 0) {
    trap(7);
  }
  iVar5 = (iVar4 / (int)uVar14) * uVar6;
  uVar10 = (long)(iVar4 % (int)uVar14 << 0x10) | uVar16 & 0xffff;
  if (uVar10 < (uint32_t)(long)iVar5) {
    iVar4 = (int)uVar10 + iVar17;
    uVar10 = (uint32_t)iVar4;
    if ((uVar8 <= uVar10) && (uVar10 < (uint32_t)(long)iVar5)) {
      uVar10 = (long)(iVar4 + iVar17);
    }
  }
  if (0x00000000u != (uint32_t)0x20) {
    (*(uint32_t*)rtguest::G((uint32_t)uStack_20)) = (uint32_t)((uint32_t)((int)uVar10 - iVar5) >> (uVar13 & 0x1f));
  }
LAB_0028decc:
  if (iVar18 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)uStack_20)) = CONCAT44(-(uint32_t)(-(int)(*(uint32_t*)rtguest::G((uint32_t)uStack_20)) != 0) - (int)((*(uint32_t*)rtguest::G((uint32_t)uStack_20)) >> 0x20),-(int)(*(uint32_t*)rtguest::G((uint32_t)uStack_20)));
  }
}


RT_GUEST_FN(0x0028d8a8, guest::__moddi3)

}  // namespace guest
