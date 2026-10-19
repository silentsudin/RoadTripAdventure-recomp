// guest:0x00277340 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t floor(uint32_t __x, ...) {
  undefined8 uVar1;
  long lVar2;
  double dVar3;
  int iVar4;
  ulong uVar5;
  uint uVar6;
  uint uVar7;
  ulong uVar8;
  uint uVar9;
  ulong uVar10;
  
  uVar7 = SUB84(__x,0);
  uVar8 = (uint32_t)(int)uVar7;
  uVar5 = (long)__x >> 0x20;
  iVar4 = (int)((uint32_t)__x >> 0x20);
  uVar9 = iVar4 >> 0x14 & 0x7ff;
  uVar6 = uVar9 - 0x3ff;
  if ((int)uVar6 < 0x14) {
    if ((int)uVar6 < 0) {
      uVar1 = dpadd(__x,0x7e37e43c8800759c);
      lVar2 = dpcmp(uVar1,0);
      if (0 < lVar2) {
        if ((long)uVar5 < 0) {
          if ((uVar5 & 0x7fffffff) != 0 || uVar8 != 0) {
            uVar5 = 0xffffffffbff00000;
            uVar8 = 0;
          }
        }
        else {
          uVar8 = 0;
          uVar5 = 0;
        }
      }
    }
    else {
      uVar10 = (uint32_t)(0xfffff >> (uVar6 & 0x1f));
      if ((uVar5 & uVar10) == 0 && uVar8 == 0) {
      }
      uVar1 = dpadd(__x,0x7e37e43c8800759c);
      lVar2 = dpcmp(uVar1,0);
      if (0 < lVar2) {
        if ((long)uVar5 < 0) {
          uVar5 = (uint32_t)(iVar4 + (0x100000 >> (uVar6 & 0x1f)));
        }
        uVar8 = 0;
        uVar5 = uVar5 & ~uVar10;
      }
    }
  }
  else {
    if (0x33 < (int)uVar6) {
      if (uVar6 != 0x400) {
      }
      dVar3 = (double)dpadd(__x,__x);
    }
    uVar9 = 0xffffffff >> (uVar9 - 0x413 & 0x1f);
    if ((uVar7 & uVar9) == 0) {
    }
    uVar1 = dpadd(__x,0x7e37e43c8800759c);
    lVar2 = dpcmp(uVar1,0);
    if (0 < lVar2) {
      uVar10 = uVar8;
      if ((long)uVar5 < 0) {
        if (uVar6 == 0x14) {
          iVar4 = iVar4 + 1;
        }
        else {
          uVar10 = (uint32_t)(int)(uVar7 + (1 << (0x34 - uVar6 & 0x1f)));
          iVar4 = iVar4 + (uint32_t)(uVar10 < uVar8);
        }
        uVar5 = (uint32_t)iVar4;
      }
      uVar8 = uVar10 & ~(long)(int)uVar9;
    }
  }
  return (double)(uVar5 << 0x20 | uVar8 & 0xffffffff);
}


RT_GUEST_FN(0x00277340, guest::floor)

}  // namespace guest
