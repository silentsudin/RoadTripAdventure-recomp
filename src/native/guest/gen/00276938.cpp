// guest:0x00276938 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __kernel_rem_pio2f(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
uint32_t auStack_1b4 = rtguest::allocFrame(84);
uint32_t afStack_160 = rtguest::allocFrame(76);
uint32_t afStack_114 = rtguest::allocFrame(84);
uint32_t afStack_c0 = rtguest::allocFrame(80);
rtguest::allocFrame(4);  // guest frame gap
    uint32_t iStack_6c = rtguest::allocFrame(4);
    uint32_t iStack_68 = rtguest::allocFrame(4);
  uint uVar1;
uint32_t piVar2;
uint32_t pfVar3;
  int iVar4;
uint32_t pfVar5;
uint32_t pfVar6;
uint32_t pfVar7;
  uint uVar8;
uint32_t puVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  int iVar13;
  int iVar14;
  bool bVar15;
  int iVar16;
  int iVar17;
  int iVar18;
  float fVar19;
  float fVar20;
  uint uVar21;
  float fVar22;
uint32_t pfStack_70;
  
  iVar18 = param_3 + -3;
  if (param_3 + -3 < 0) {
    iVar18 = param_3 + 4;
  }
  iVar18 = iVar18 >> 3;
  (*(int32_t*)rtguest::G((uint32_t)iStack_6c)) = param_5;
  if (iVar18 < 0) {
    iVar18 = 0;
  }
  iVar14 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x00331a90 + param_5 * 4)));
  iVar17 = param_4 + -1;
  param_3 = param_3 + iVar18 * -8;
  iVar16 = param_3 + -8;
  iVar11 = iVar18 - iVar17;
  pfStack_70 = param_1;
  (*(int32_t*)rtguest::G((uint32_t)iStack_68)) = param_6;
  if (-1 < iVar17 + iVar14) {
    iVar13 = iVar17 + iVar14 + 1;
    piVar2 = (uint32_t)(iVar11 * 4 + param_6);
    pfVar6 = afStack_160;
    do {
      if (iVar11 < 0) {
        (*(float*)rtguest::G((uint32_t)pfVar6)) = 0.0;
      }
      else {
        (*(float*)rtguest::G((uint32_t)pfVar6)) = (float)(*(int32_t*)rtguest::G((uint32_t)piVar2));
      }
      iVar13 = iVar13 + -1;
      pfVar6 = pfVar6 + 0x4;  // T* element step
      piVar2 = piVar2 + 0x4;  // T* element step
      iVar11 = iVar11 + 1;
    } while (iVar13 != 0);
  }
  iVar13 = 0;
  iVar11 = iVar14;
  if (-1 < iVar14) {
    pfVar6 = afStack_c0;
    iVar10 = iVar17;
    do {
      fVar22 = 0.0;
      if (-1 < iVar17) {
        pfVar3 = afStack_160 + iVar10;
        pfVar7 = pfStack_70;
        iVar12 = param_4;
        do {
          fVar19 = (*(float*)rtguest::G((uint32_t)pfVar7));
          pfVar7 = pfVar7 + 0x4;  // T* element step
          fVar20 = (*(float*)rtguest::G((uint32_t)pfVar3));
          pfVar3 = pfVar3 + -1;
          iVar12 = iVar12 + -1;
          fVar22 = fVar22 + fVar19 * fVar20;
        } while (iVar12 != 0);
      }
      iVar13 = iVar13 + 1;
      (*(float*)rtguest::G((uint32_t)pfVar6)) = fVar22;
      iVar10 = iVar10 + 1;
      pfVar6 = pfVar6 + 0x4;  // T* element step
    } while (iVar13 <= iVar14);
  }
  do {
    pfVar6 = afStack_c0 + iVar11;
    fVar22 = (*(float*)rtguest::G((uint32_t)pfVar6));
    puVar9 = auStack_1b4 + 1;
    for (iVar13 = iVar11; 0 < iVar13; iVar13 = iVar13 + -1) {
      pfVar6 = pfVar6 + -1;
      fVar20 = fVar22 - (float)(int)(fVar22 * 0.00390625) * 256.0;
      fVar22 = (*(float*)rtguest::G((uint32_t)pfVar6)) + (float)(int)(fVar22 * 0.00390625);
      (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = (int)fVar20;
      puVar9 = puVar9 + 0x4;  // T* element step
    }
    fVar22 = (float)scalbnf(fVar22,iVar16);
    iVar13 = 0;
    fVar20 = (float)floorf(fVar22 * 0.125);
    fVar22 = fVar22 - fVar20 * 8.0;
    uVar21 = (uint32_t)fVar22;
    fVar22 = fVar22 - (float)(int)uVar21;
    if (iVar16 < 1) {
      if (iVar16 == 0) {
        iVar13 = (int)(*(uint32_t*)rtguest::G((uint32_t)auStack_1b4 + (uint32_t)(iVar11) * 4)) >> 8;
      }
      else if (0.5 <= fVar22) {
        iVar13 = 2;
      }
    }
    else {
      uVar8 = (*(uint32_t*)rtguest::G((uint32_t)auStack_1b4 + (uint32_t)(iVar11) * 4));
      iVar13 = (int)uVar8 >> (8U - iVar16 & 0x1f);
      uVar21 = uVar21 + iVar13;
      uVar8 = uVar8 - (iVar13 << (8U - iVar16 & 0x1f));
      (*(uint32_t*)rtguest::G((uint32_t)auStack_1b4 + (uint32_t)(iVar11) * 4)) = uVar8;
      iVar13 = (int)uVar8 >> (7U - iVar16 & 0x1f);
    }
    if (0 < iVar13) {
      uVar21 = uVar21 + 1;
      bVar15 = false;
      puVar9 = auStack_1b4 + 1;
      iVar10 = iVar11;
      if (0 < iVar11) {
        do {
          iVar12 = (*(uint32_t*)rtguest::G((uint32_t)puVar9));
          if (bVar15) {
            iVar4 = 0xff - iVar12;
LAB_00276bec:
            (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = iVar4;
          }
          else {
            iVar4 = 0x100 - iVar12;
            if (iVar12 != 0) {
              bVar15 = true;
              goto LAB_00276bec;
            }
          }
          iVar10 = iVar10 + -1;
          puVar9 = puVar9 + 0x4;  // T* element step
        } while (iVar10 != 0);
      }
      if (0 < iVar16) {
        if (iVar16 == 1) {
          puVar9 = auStack_1b4 + iVar11;
          uVar8 = (*(uint32_t*)rtguest::G((uint32_t)puVar9)) & 0x7f;
        }
        else {
          if (iVar16 != 2) goto LAB_00276c54;
          puVar9 = auStack_1b4 + iVar11;
          uVar8 = (*(uint32_t*)rtguest::G((uint32_t)puVar9)) & 0x3f;
        }
        (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = uVar8;
      }
LAB_00276c54:
      if ((iVar13 == 2) && (fVar22 = 1.0 - fVar22, bVar15)) {
        fVar20 = (float)scalbnf(iVar16);
        fVar22 = fVar22 - fVar20;
      }
    }
    iVar10 = iVar11 + -1;
    if (fVar22 != 0.0) {
LAB_00276dd8:
      iVar18 = iVar11;
      if (fVar22 == 0.0) {
        puVar9 = auStack_1b4 + iVar11;
        param_3 = param_3 + -0x10;
        uVar8 = (*(uint32_t*)rtguest::G((uint32_t)puVar9));
        while (iVar18 = iVar18 + -1, uVar8 == 0) {
          puVar9 = puVar9 + -1;
          param_3 = param_3 + -8;
          uVar8 = (*(uint32_t*)rtguest::G((uint32_t)puVar9));
        }
      }
      else {
        fVar22 = (float)scalbnf(fVar22,-iVar16);
        if (256.0 <= fVar22) {
          iVar18 = iVar11 + 1;
          (*(uint32_t*)rtguest::G((uint32_t)auStack_1b4 + (uint32_t)(iVar11 + 1) * 4)) = (int)(fVar22 - (float)(int)(fVar22 * 0.00390625) * 256.0);
          (*(uint32_t*)rtguest::G((uint32_t)auStack_1b4 + (uint32_t)(iVar11 + 2) * 4)) = (int)(float)(int)(fVar22 * 0.00390625);
        }
        else {
          (*(uint32_t*)rtguest::G((uint32_t)auStack_1b4 + (uint32_t)(iVar11 + 1) * 4)) = (int)fVar22;
          param_3 = iVar16;
        }
      }
      fVar22 = (float)scalbnf(0x3f800000,param_3);
      for (iVar11 = iVar18; -1 < iVar11; iVar11 = iVar11 + -1) {
        fVar20 = fVar22 * (float)(int)(*(uint32_t*)rtguest::G((uint32_t)auStack_1b4 + (uint32_t)(iVar11 + 1) * 4));
        fVar22 = fVar22 * 0.00390625;
        (*(float*)rtguest::G((uint32_t)afStack_c0 + (uint32_t)(iVar11) * 4)) = fVar20;
      }
      if (-1 < iVar18) {
        iVar11 = iVar18;
        do {
          fVar22 = 0.0;
          iVar16 = 0;
          if (iVar14 < 0) {
            iVar17 = iVar18 - iVar11;
          }
          else {
            iVar17 = iVar18 - iVar11;
            if (-1 < iVar17) {
              pfVar7 = afStack_c0 + iVar11;
              fVar20 = 1.5703125;
              pfVar6 = (uint32_t)0x00331aa0;
              while( true ) {
                pfVar6 = pfVar6 + 0x4;  // T* element step
                fVar19 = (*(float*)rtguest::G((uint32_t)pfVar7));
                pfVar7 = pfVar7 + 0x4;  // T* element step
                iVar16 = iVar16 + 1;
                fVar22 = fVar22 + fVar20 * fVar19;
                if (iVar14 < iVar16) break;
                if (iVar17 < iVar16) {
                  iVar17 = iVar17 * 4;
                  goto LAB_00276f90;
                }
                fVar20 = (*(float*)rtguest::G((uint32_t)pfVar6));
              }
            }
          }
          iVar17 = iVar17 << 2;
LAB_00276f90:
          iVar11 = iVar11 + -1;
          (*(float*)rtguest::G((uint32_t)(uint32_t)((int)afStack_114 + iVar17 + 4))) = fVar22;
        } while (-1 < iVar11);
      }
      if ((*(int32_t*)rtguest::G((uint32_t)iStack_6c)) < 3) {
        if ((*(int32_t*)rtguest::G((uint32_t)iStack_6c)) < 1) {
          if ((*(int32_t*)rtguest::G((uint32_t)iStack_6c)) == 0) {
            fVar22 = 0.0;
            if (-1 < iVar18) {
              pfVar6 = afStack_114 + iVar18 + 1;
              do {
                fVar20 = (*(float*)rtguest::G((uint32_t)pfVar6));
                pfVar6 = pfVar6 + -1;
                iVar18 = iVar18 + -1;
                fVar22 = fVar22 + fVar20;
              } while (-1 < iVar18);
            }
            (*(float*)rtguest::G((uint32_t)param_2)) = fVar22;
            if (iVar13 != 0) {
              (*(float*)rtguest::G((uint32_t)param_2)) = -fVar22;
            }
          }
        }
        else {
          fVar22 = 0.0;
          if (-1 < iVar18) {
            pfVar6 = afStack_114 + iVar18 + 1;
            iVar14 = iVar18;
            do {
              fVar20 = (*(float*)rtguest::G((uint32_t)pfVar6));
              pfVar6 = pfVar6 + -1;
              iVar14 = iVar14 + -1;
              fVar22 = fVar22 + fVar20;
            } while (-1 < iVar14);
          }
          (*(float*)rtguest::G((uint32_t)param_2)) = fVar22;
          if (iVar13 != 0) {
            (*(float*)rtguest::G((uint32_t)param_2)) = -fVar22;
          }
          (*(float*)rtguest::G((uint32_t)afStack_114 + (uint32_t)(1) * 4)) = (*(float*)rtguest::G((uint32_t)afStack_114 + (uint32_t)(1) * 4)) - fVar22;
          if (0 < iVar18) {
            pfVar6 = afStack_114 + 2;
            do {
              fVar22 = (*(float*)rtguest::G((uint32_t)pfVar6));
              pfVar6 = pfVar6 + 0x4;  // T* element step
              iVar18 = iVar18 + -1;
              (*(float*)rtguest::G((uint32_t)afStack_114 + (uint32_t)(1) * 4)) = (*(float*)rtguest::G((uint32_t)afStack_114 + (uint32_t)(1) * 4)) + fVar22;
            } while (iVar18 != 0);
          }
          (*(float*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(float))) = (*(float*)rtguest::G((uint32_t)afStack_114 + (uint32_t)(1) * 4));
          if (iVar13 != 0) {
            (*(float*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(float))) = -(*(float*)rtguest::G((uint32_t)afStack_114 + (uint32_t)(1) * 4));
          }
        }
      }
      else if ((*(int32_t*)rtguest::G((uint32_t)iStack_6c)) == 3) {
        if (0 < iVar18) {
          pfVar6 = afStack_114 + iVar18;
          iVar14 = iVar18;
          do {
            fVar22 = (*(float*)rtguest::G((uint32_t)pfVar6));
            iVar14 = iVar14 + -1;
            fVar20 = fVar22 + (*(float*)rtguest::G((uint32_t)pfVar6 + (uint32_t)(1) * sizeof(float)));
            (*(float*)rtguest::G((uint32_t)pfVar6)) = fVar20;
            (*(float*)rtguest::G((uint32_t)pfVar6 + (uint32_t)(1) * sizeof(float))) = (*(float*)rtguest::G((uint32_t)pfVar6 + (uint32_t)(1) * sizeof(float))) + (fVar22 - fVar20);
            pfVar6 = pfVar6 + -1;
          } while (0 < iVar14);
        }
        if (1 < iVar18) {
          pfVar6 = afStack_114 + iVar18;
          iVar14 = iVar18;
          do {
            fVar22 = (*(float*)rtguest::G((uint32_t)pfVar6));
            iVar14 = iVar14 + -1;
            fVar20 = fVar22 + (*(float*)rtguest::G((uint32_t)pfVar6 + (uint32_t)(1) * sizeof(float)));
            (*(float*)rtguest::G((uint32_t)pfVar6)) = fVar20;
            (*(float*)rtguest::G((uint32_t)pfVar6 + (uint32_t)(1) * sizeof(float))) = (*(float*)rtguest::G((uint32_t)pfVar6 + (uint32_t)(1) * sizeof(float))) + (fVar22 - fVar20);
            pfVar6 = pfVar6 + -1;
          } while (1 < iVar14);
        }
        fVar22 = 0.0;
        if (1 < iVar18) {
          pfVar6 = afStack_114 + iVar18 + 1;
          do {
            fVar20 = (*(float*)rtguest::G((uint32_t)pfVar6));
            pfVar6 = pfVar6 + -1;
            iVar18 = iVar18 + -1;
            fVar22 = fVar22 + fVar20;
          } while (1 < iVar18);
        }
        if (iVar13 != 0) {
          fVar22 = -fVar22;
          (*(float*)rtguest::G((uint32_t)afStack_114 + (uint32_t)(1) * 4)) = -(*(float*)rtguest::G((uint32_t)afStack_114 + (uint32_t)(1) * 4));
          (*(float*)rtguest::G((uint32_t)afStack_114 + (uint32_t)(2) * 4)) = -(*(float*)rtguest::G((uint32_t)afStack_114 + (uint32_t)(2) * 4));
        }
        (*(float*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(float))) = fVar22;
        (*(float*)rtguest::G((uint32_t)param_2)) = (*(float*)rtguest::G((uint32_t)afStack_114 + (uint32_t)(1) * 4));
        (*(float*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(float))) = (*(float*)rtguest::G((uint32_t)afStack_114 + (uint32_t)(2) * 4));
      }
      return uVar21 & 7;
    }
    uVar8 = 0;
    if (iVar14 <= iVar10) {
      puVar9 = auStack_1b4 + iVar11;
      do {
        uVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar9));
        puVar9 = puVar9 + -1;
        iVar10 = iVar10 + -1;
        uVar8 = uVar8 | uVar1;
      } while (iVar14 <= iVar10);
    }
    if (uVar8 != 0) goto LAB_00276dd8;
    iVar13 = 1;
    if ((*(uint32_t*)rtguest::G((uint32_t)auStack_1b4 + (uint32_t)(iVar14) * 4)) == 0) {
      puVar9 = auStack_1b4 + iVar14;
      do {
        puVar9 = puVar9 + -1;
        iVar13 = iVar13 + 1;
      } while ((*(uint32_t*)rtguest::G((uint32_t)puVar9)) == 0);
    }
    iVar13 = iVar11 + iVar13;
    iVar10 = iVar11 + 1;
    iVar11 = iVar13;
    if (iVar10 <= iVar13) {
      piVar2 = (uint32_t)((iVar18 + iVar10) * 4 + (*(int32_t*)rtguest::G((uint32_t)iStack_68)));
      pfVar6 = afStack_c0 + iVar10;
      pfVar7 = afStack_160 + iVar17 + iVar10;
      do {
        fVar22 = 0.0;
        (*(float*)rtguest::G((uint32_t)pfVar7)) = (float)(*(int32_t*)rtguest::G((uint32_t)piVar2));
        if (-1 < iVar17) {
          pfVar5 = afStack_160 + iVar17 + iVar10;
          pfVar3 = pfStack_70;
          iVar12 = param_4;
          do {
            fVar19 = (*(float*)rtguest::G((uint32_t)pfVar3));
            pfVar3 = pfVar3 + 0x4;  // T* element step
            fVar20 = (*(float*)rtguest::G((uint32_t)pfVar5));
            pfVar5 = pfVar5 + -1;
            iVar12 = iVar12 + -1;
            fVar22 = fVar22 + fVar19 * fVar20;
          } while (iVar12 != 0);
        }
        iVar10 = iVar10 + 1;
        (*(float*)rtguest::G((uint32_t)pfVar6)) = fVar22;
        pfVar7 = pfVar7 + 0x4;  // T* element step
        pfVar6 = pfVar6 + 0x4;  // T* element step
        piVar2 = piVar2 + 0x4;  // T* element step
      } while (iVar10 <= iVar13);
    }
  } while( true );
}


RT_GUEST_FN(0x00276938, guest::__kernel_rem_pio2f)

}  // namespace guest
