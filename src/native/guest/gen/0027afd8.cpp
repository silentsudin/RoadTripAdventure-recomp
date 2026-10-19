// guest:0x0027afd8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#define __umoddi3 __umoddi3_decl
#include "guest_decls.inc"
#undef __umoddi3

namespace guest {

uint64_t __umoddi3(uint64_t param_1, uint64_t param_2, ...) {
  uint64_t uStack_10;
  long lVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  ulong uVar9;
  uint uVar10;
  ulong uVar11;
  ulong uVar12;
  uint uVar13;
  uint uVar14;
  uint uVar15;
  ulong uVar16;
  int iVar17;
  
  uVar11 = param_2 >> 0x20;
  uVar12 = (long)param_1 >> 0x20;
  uVar7 = (uint32_t)param_2;
  uVar9 = (uint64_t)(int)uVar7;
  uVar14 = (uint32_t)param_1;
  uVar16 = (uint64_t)(int)uVar14;
  uVar6 = (uint32_t)(param_1 >> 0x20);
  uVar13 = (uint32_t)(param_2 >> 0x20);
  if (uVar11 != 0) {
    if (uVar12 < uVar11) {
      return param_1 & 0xffffffff | uVar12 << 0x20;
    }
    if (uVar11 < 0x10000) {
      iVar4 = 8;
      if (uVar11 < 0x100) {
        iVar4 = 0;
      }
    }
    else {
      iVar4 = 0x10;
      if (0xffffff < uVar11) {
        iVar4 = 0x18;
      }
    }
    uVar3 = 0x20 - ((uint32_t)(uint32_t)(*(uint8_t*)rtguest::G((uint32_t)0x331e70 + (uint32_t)(uVar13 >> iVar4) * 1)) + iVar4);
    uVar8 = 0x20 - uVar3;
    if (uVar3 == 0) {
      if ((uVar11 < uVar12) || (uVar11 = uVar16, uVar9 <= uVar16)) {
        uVar11 = (uint64_t)(int)(uVar14 - uVar7);
        uVar12 = (uint64_t)(int)((uVar6 - uVar13) - (uint32_t)(uVar16 < uVar11));
      }
      if (0x00000000u == (uint32_t)0x10) {
      }
      return uVar11 & 0xffffffff | uVar12 << 0x20;
    }
    uVar15 = uVar14 << (uVar3 & 0x1f);
    uVar10 = uVar13 << (uVar3 & 0x1f) | uVar7 >> (uVar8 & 0x1f);
    uVar13 = uVar6 >> (uVar8 & 0x1f);
    uVar7 = uVar7 << (uVar3 & 0x1f);
    uVar14 = uVar6 << (uVar3 & 0x1f) | uVar14 >> (uVar8 & 0x1f);
    uVar6 = uVar10 >> 0x10;
    iVar4 = (int)uVar13 / (int)uVar6;
    if (uVar6 == 0) {
      trap(7);
    }
    uVar2 = iVar4 * (uVar10 & 0xffff);
    uVar13 = (int)uVar13 % (int)uVar6 << 0x10 | uVar14 >> 0x10;
    if (uVar13 < uVar2) {
      uVar13 = uVar13 + uVar10;
      iVar17 = iVar4 + -1;
      if (uVar10 <= uVar13) {
        if (uVar2 <= uVar13) {
          iVar5 = uVar13 - uVar2;
          goto LAB_0027b43c;
        }
        iVar17 = iVar4 + -2;
        uVar13 = uVar13 + uVar10;
      }
      iVar5 = uVar13 - uVar2;
    }
    else {
      iVar5 = uVar13 - uVar2;
      iVar17 = iVar4;
    }
LAB_0027b43c:
    if (uVar6 == 0) {
      trap(7);
    }
    uVar13 = iVar5 / (int)uVar6;
    uVar2 = uVar13 * (uVar10 & 0xffff);
    uVar6 = iVar5 % (int)uVar6 << 0x10 | uVar14 & 0xffff;
    if (uVar6 < uVar2) {
      uVar6 = uVar6 + uVar10;
      uVar14 = uVar13 - 1;
      if (uVar10 <= uVar6) {
        if (uVar2 <= uVar6) {
          uVar6 = uVar6 - uVar2;
          goto LAB_0027b498;
        }
        uVar14 = uVar13 - 2;
        uVar6 = uVar6 + uVar10;
      }
      uVar6 = uVar6 - uVar2;
    }
    else {
      uVar6 = uVar6 - uVar2;
      uVar14 = uVar13;
    }
LAB_0027b498:
    lVar1 = (uint64_t)(iVar17 << 0x10 | uVar14) * (uint64_t)uVar7;
    uVar14 = (uint32_t)lVar1;
    uVar13 = (uint32_t)((uint64_t)lVar1 >> 0x20);
    if ((uVar6 < uVar13) || ((uVar13 == uVar6 && (uVar15 < uVar14)))) {
      uVar7 = uVar14 - uVar7;
      lVar1 = CONCAT44((uVar13 - uVar10) - (uint32_t)(uVar14 < uVar7),uVar7);
    }
    uVar7 = uVar15 - (int)lVar1;
    if (0x00000000u == (uint32_t)0x10) {
    }
    uVar13 = (uVar6 - (int)((uint64_t)lVar1 >> 0x20)) - (uint32_t)(uVar15 < uVar7);
    return CONCAT44(uVar13 >> (uVar3 & 0x1f),uVar13 << (uVar8 & 0x1f) | uVar7 >> (uVar3 & 0x1f));
  }
  if (uVar12 < uVar9) {
    if (uVar9 < 0x10000) {
      iVar4 = 8;
      if (uVar9 < 0x100) {
        iVar4 = 0;
      }
    }
    else {
      iVar4 = 0x10;
      if (0xffffff < uVar9) {
        iVar4 = 0x18;
      }
    }
    uVar13 = 0x20 - ((uint32_t)(uint32_t)(*(uint8_t*)rtguest::G((uint32_t)0x331e70 + (uint32_t)(uVar7 >> iVar4) * 1)) + iVar4);
    if (uVar13 != 0) {
      uVar16 = (uint64_t)(int)(uVar14 << (uVar13 & 0x1f));
      uVar12 = (uint64_t)(int)(uVar6 << (uVar13 & 0x1f) | uVar14 >> (0x20 - uVar13 & 0x1f));
      uVar9 = (uint64_t)(int)(uVar7 << (uVar13 & 0x1f));
    }
    iVar5 = (int)uVar12;
    uVar14 = (uint32_t)uVar9 >> 0x10;
    uVar7 = (uint32_t)uVar9 & 0xffff;
    goto LAB_0027b1e0;
  }
  if (uVar9 == 0) {
    trap(7);
    uVar9 = (uint64_t)(1 / (int)uVar13);
  }
  if (uVar9 < 0x10000) {
    iVar4 = 8;
    if (uVar9 < 0x100) {
      iVar4 = 0;
    }
  }
  else {
    iVar4 = 0x10;
    if (0xffffff < uVar9) {
      iVar4 = 0x18;
    }
  }
  uVar7 = (uint32_t)uVar9;
  uVar13 = 0x20 - ((uint32_t)(uint32_t)(*(uint8_t*)rtguest::G((uint32_t)0x331e70 + (uint32_t)(uVar7 >> iVar4) * 1)) + iVar4);
  if (uVar13 == 0) {
    iVar5 = uVar6 - uVar7;
    uVar14 = uVar7 >> 0x10;
    uVar7 = uVar7 & 0xffff;
    goto LAB_0027b1e0;
  }
  uVar16 = (uint64_t)(int)(uVar14 << (uVar13 & 0x1f));
  uVar3 = uVar6 >> (0x20 - uVar13 & 0x1f);
  uVar6 = uVar6 << (uVar13 & 0x1f) | uVar14 >> (0x20 - uVar13 & 0x1f);
  uVar8 = uVar7 << (uVar13 & 0x1f);
  uVar9 = (uint32_t)(int)uVar8;
  uVar14 = uVar8 >> 0x10;
  uVar7 = uVar8 & 0xffff;
  if (uVar14 == 0) {
    trap(7);
  }
  iVar4 = ((int)uVar3 / (int)uVar14) * uVar7;
  uVar3 = (int)uVar3 % (int)uVar14 << 0x10 | uVar6 >> 0x10;
  if ((uint64_t)(long)(int)uVar3 < (uint64_t)(long)iVar4) {
    uVar3 = uVar3 + uVar8;
    if (uVar9 <= (uint64_t)(long)(int)uVar3) {
      if ((uint64_t)(long)(int)uVar3 < (uint64_t)(long)iVar4) {
        uVar3 = uVar3 + uVar8;
      }
      goto LAB_0027b180;
    }
    iVar4 = uVar3 - iVar4;
  }
  else {
LAB_0027b180:
    iVar4 = uVar3 - iVar4;
  }
  if (uVar14 == 0) {
    trap(7);
  }
  iVar17 = (iVar4 / (int)uVar14) * uVar7;
  uVar11 = (long)(iVar4 % (int)uVar14 << 0x10) | (long)(int)uVar6 & 0xffffU;
  iVar4 = (int)uVar11;
  iVar5 = iVar4 - iVar17;
  if (uVar11 < (uint64_t)(long)iVar17) {
    iVar4 = iVar4 + uVar8;
    iVar5 = iVar4 - iVar17;
    if (uVar9 <= (uint64_t)(long)iVar4) {
      if ((uint64_t)(long)iVar4 < (uint64_t)(long)iVar17) {
        iVar4 = iVar4 + uVar8;
      }
      iVar5 = iVar4 - iVar17;
    }
  }
LAB_0027b1e0:
  if (uVar14 == 0) {
    trap(7);
  }
  iVar4 = (iVar5 / (int)uVar14) * uVar7;
  uVar6 = iVar5 % (int)uVar14 << 0x10 | (uint32_t)uVar16 >> 0x10;
  iVar17 = (int)uVar9;
  if ((uint32_t)(long)(int)uVar6 < (uint64_t)(long)iVar4) {
    iVar5 = uVar6 + iVar17;
    if ((uint64_t)(long)iVar5 < uVar9) {
      iVar4 = iVar5 - iVar4;
    }
    else {
      if ((uint64_t)(long)iVar5 < (uint64_t)(long)iVar4) {
        iVar5 = iVar5 + iVar17;
      }
      iVar4 = iVar5 - iVar4;
    }
  }
  else {
    iVar4 = uVar6 - iVar4;
  }
  if (uVar14 == 0) {
    trap(7);
  }
  iVar5 = (iVar4 / (int)uVar14) * uVar7;
  uVar11 = (long)(iVar4 % (int)uVar14 << 0x10) | uVar16 & 0xffff;
  if (uVar11 < (uint64_t)(long)iVar5) {
    iVar4 = (int)uVar11 + iVar17;
    uVar11 = (uint64_t)iVar4;
    if ((uVar9 <= uVar11) && (uVar11 < (uint64_t)(long)iVar5)) {
      uVar11 = (long)(iVar4 + iVar17);
    }
  }
  if (0x00000000u != (uint32_t)0x10) {
    uStack_10 = (uint64_t)((uint32_t)((int)uVar11 - iVar5) >> (uVar13 & 0x1f));
  return uStack_10;
  }
}


RT_GUEST_FN(0x0027afd8, guest::__umoddi3)

}  // namespace guest
