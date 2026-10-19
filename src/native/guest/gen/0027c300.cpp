// guest:0x0027c300 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t end_fde_sort(uint32_t param_1, uint32_t param_2, ...) {
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
uint32_t puVar5;
uint32_t piVar6;
  ulong uVar7;
uint32_t piVar8;
uint32_t piVar9;
  long lVar10;
uint32_t piVar11;
  int iVar12;
  int iVar13;
  ulong uVar14;
  int iVar15;
  int iVar16;
  uint uVar17;
  ulong uVar18;
  ulong uVar19;
uint32_t puVar20;
  uint uVar21;
  uint uVar22;
uint32_t piVar23;
  long lVar24;
  
  piVar9 = (uint32_t)param_1;
  lVar24 = (long)(*(int32_t*)rtguest::G((uint32_t)piVar9));
  lVar10 = lVar24;
  if (lVar24 != 0) {
    uVar21 = (*(int32_t*)rtguest::G((uint32_t)piVar9 + (uint32_t)(1) * sizeof(int32_t)));
    if (uVar21 == param_2) {
      iVar15 = (*(int32_t*)rtguest::G((uint32_t)piVar9 + (uint32_t)(2) * sizeof(int32_t)));
      goto LAB_0027c350;
    }
    abort();
  }
  uVar21 = (*(int32_t*)rtguest::G((uint32_t)piVar9 + (uint32_t)(1) * sizeof(int32_t)));
  iVar15 = (*(int32_t*)rtguest::G((uint32_t)piVar9 + (uint32_t)(2) * sizeof(int32_t)));
LAB_0027c350:
  puVar20 = (uint32_t)lVar24;
  uVar22 = uVar21;
  if (iVar15 == 0) {
joined_r0x0027c684:
    if (uVar22 != 0) {
      uVar22 = uVar22 - 1;
      uVar17 = uVar22;
      while (iVar15 = uVar17 * 2, iVar15 + 1U < uVar21) {
        if (iVar15 + 2U < uVar21) {
          iVar16 = uVar17 * 8;
          iVar13 = (*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(uVar17 * 2 + 2) * sizeof(uint32_t)));
          iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 8)));
          if (iVar12 - (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(uVar17 * 2 + 1) * sizeof(uint32_t))) + 8))) < 1) goto LAB_0027c708;
          iVar3 = (*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(uVar17) * sizeof(uint32_t)));
          if (iVar12 - (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) < 1) goto LAB_0027c708;
          (*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(uVar17) * sizeof(uint32_t))) = iVar13;
          (*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(uVar17 * 2 + 2) * sizeof(uint32_t))) = iVar3;
          uVar17 = iVar15 + 2U;
        }
        else {
          iVar16 = uVar17 << 3;
LAB_0027c708:
          piVar9 = (uint32_t)((int)puVar20 + iVar16 + 4);
          iVar16 = (*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(uVar17) * sizeof(uint32_t)));
          iVar13 = (*(int32_t*)rtguest::G((uint32_t)piVar9));
          if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 8))) - (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar16 + 8))) < 1) break;
          (*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(uVar17) * sizeof(uint32_t))) = iVar13;
          (*(int32_t*)rtguest::G((uint32_t)piVar9)) = iVar16;
          uVar17 = iVar15 + 1;
        }
      }
      goto joined_r0x0027c684;
    }
    if (1 < uVar21) {
      puVar5 = puVar20 + uVar21;
      do {
        puVar5 = puVar5 + -1;
        uVar21 = uVar21 - 1;
        uVar2 = (*(uint32_t*)rtguest::G((uint32_t)puVar20));
        (*(uint32_t*)rtguest::G((uint32_t)puVar20)) = (*(uint32_t*)rtguest::G((uint32_t)puVar5));
        (*(uint32_t*)rtguest::G((uint32_t)puVar5)) = uVar2;
        uVar22 = 0;
        if (1 < uVar21) {
          iVar15 = 0;
          uVar17 = 2;
          do {
            if (uVar17 < uVar21) {
              iVar16 = uVar22 * 8;
              iVar13 = (*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(uVar22 * 2 + 2) * sizeof(uint32_t)));
              iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 8)));
              if (iVar12 - (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(uVar22 * 2 + 1) * sizeof(uint32_t))) + 8))) < 1) goto LAB_0027c800;
              iVar3 = (*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(uVar22) * sizeof(uint32_t)));
              if (iVar12 - (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) < 1) goto LAB_0027c800;
              (*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(uVar22) * sizeof(uint32_t))) = iVar13;
              (*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(uVar22 * 2 + 2) * sizeof(uint32_t))) = iVar3;
              uVar22 = uVar17;
            }
            else {
              iVar16 = uVar22 << 3;
LAB_0027c800:
              piVar9 = (uint32_t)((int)puVar20 + iVar16 + 4);
              iVar16 = (*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(uVar22) * sizeof(uint32_t)));
              iVar13 = (*(int32_t*)rtguest::G((uint32_t)piVar9));
              if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 8))) - (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar16 + 8))) < 1) break;
              (*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(uVar22) * sizeof(uint32_t))) = iVar13;
              (*(int32_t*)rtguest::G((uint32_t)piVar9)) = iVar16;
              uVar22 = iVar15 + 1;
            }
            iVar15 = uVar22 * 2;
            uVar17 = iVar15 + 2;
          } while (iVar15 + 1U < uVar21);
        }
      } while (1 < uVar21);
    }
  }
  else {
    piVar23 = piVar9 + 2;
    piVar11 = 0x0177615c;
    uVar22 = 0;
    if (uVar21 != 0) {
      iVar15 = (*(int32_t*)rtguest::G((uint32_t)piVar23));
      iVar16 = 0;
      do {
        if ((piVar11 != 0x0177615c) &&
           ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar16 + (int)puVar20))) + 8))) - (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)piVar11)) + 8))) < 0)) {
          iVar13 = (int)piVar11 - (int)puVar20;
          do {
            puVar5 = (uint32_t)(iVar15 + iVar13);
            piVar11 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar5));
            (*(uint32_t*)rtguest::G((uint32_t)puVar5)) = 0;
            if (piVar11 == 0x0177615c) break;
            iVar13 = (int)piVar11 - (int)puVar20;
          } while ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar16 + (int)puVar20))) + 8))) - (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)piVar11)) + 8))) < 0);
        }
        uVar22 = uVar22 + 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar16 + iVar15))) = piVar11;
        piVar11 = (uint32_t)((int)puVar20 + iVar16);
        iVar16 = iVar16 + 4;
      } while (uVar22 < uVar21);
    }
    iVar15 = 0;
    uVar22 = 0;
    uVar14 = 0;
    if (uVar21 != 0) {
      piVar11 = (uint32_t)(*(int32_t*)rtguest::G((uint32_t)piVar23));
      lVar10 = lVar24;
      piVar8 = piVar11;
      do {
        iVar16 = (*(int32_t*)rtguest::G((uint32_t)piVar8));
        piVar8 = piVar8 + 1;
        piVar6 = (uint32_t)lVar24;
        if (iVar16 == 0) {
          uVar14 = (uint32_t)((int)uVar14 + 1);
          (*(int32_t*)rtguest::G((uint32_t)piVar11)) = (*(int32_t*)rtguest::G((uint32_t)piVar6));
          piVar11 = piVar11 + 1;
        }
        else {
          iVar15 = iVar15 + 1;
          (*(int32_t*)rtguest::G((uint32_t)(uint32_t)lVar10)) = (*(int32_t*)rtguest::G((uint32_t)piVar6));
          lVar10 = (long)(int)((uint32_t)lVar10 + 1);
        }
        uVar22 = uVar22 + 1;
        lVar24 = (long)(int)(piVar6 + 1);
      } while (uVar22 < uVar21);
    }
    (*(int32_t*)rtguest::G((uint32_t)piVar9 + (uint32_t)(1) * sizeof(int32_t))) = iVar15;
    (*(int32_t*)rtguest::G((uint32_t)piVar9 + (uint32_t)(3) * sizeof(int32_t))) = (int)uVar14;
    if ((*(int32_t*)rtguest::G((uint32_t)piVar9 + (uint32_t)(1) * sizeof(int32_t))) + (*(int32_t*)rtguest::G((uint32_t)piVar9 + (uint32_t)(3) * sizeof(int32_t))) != param_2) {
      abort();
    }
    puVar20 = (uint32_t)(*(int32_t*)rtguest::G((uint32_t)piVar23));
    iVar15 = (int)uVar14;
    uVar7 = uVar14;
joined_r0x0027c494:
    if (uVar7 != 0) {
      iVar15 = iVar15 + -1;
      uVar7 = (uint32_t)iVar15;
      uVar19 = uVar7;
      iVar16 = iVar15;
      while (iVar16 = iVar16 * 2, (uint32_t)(long)(iVar16 + 1) < uVar14) {
        uVar18 = (uint32_t)(iVar16 + 2);
        iVar13 = (int)uVar19;
        if (uVar18 < uVar14) {
          iVar12 = iVar13 * 8;
          iVar3 = (*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(iVar13 * 2 + 2) * sizeof(uint32_t)));
          iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8)));
          if (iVar4 - (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(iVar13 * 2 + 1) * sizeof(uint32_t))) + 8))) < 1) goto LAB_0027c518;
          iVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(iVar13) * sizeof(uint32_t)));
          if (iVar4 - (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 8))) < 1) goto LAB_0027c518;
          (*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(iVar13) * sizeof(uint32_t))) = iVar3;
          (*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(iVar13 * 2 + 2) * sizeof(uint32_t))) = iVar1;
        }
        else {
          iVar12 = iVar13 << 3;
LAB_0027c518:
          piVar11 = (uint32_t)((int)puVar20 + iVar12 + 4);
          iVar12 = (*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(iVar13) * sizeof(uint32_t)));
          iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar11));
          if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) - (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 8))) < 1) break;
          (*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(iVar13) * sizeof(uint32_t))) = iVar3;
          (*(int32_t*)rtguest::G((uint32_t)piVar11)) = iVar12;
          uVar18 = (uint32_t)(iVar16 + 1);
        }
        uVar19 = uVar18;
        iVar16 = (int)uVar18;
      }
      goto joined_r0x0027c494;
    }
    if (1 < uVar14) {
      puVar5 = puVar20 + (int)uVar14;
      do {
        puVar5 = puVar5 + -1;
        uVar14 = (uint32_t)((int)uVar14 + -1);
        uVar2 = (*(uint32_t*)rtguest::G((uint32_t)puVar20));
        (*(uint32_t*)rtguest::G((uint32_t)puVar20)) = (*(uint32_t*)rtguest::G((uint32_t)puVar5));
        (*(uint32_t*)rtguest::G((uint32_t)puVar5)) = uVar2;
        if (1 < uVar14) {
          iVar15 = 0;
          iVar16 = 2;
          uVar7 = 0;
          do {
            uVar19 = (uint32_t)iVar16;
            iVar16 = (int)uVar7;
            if (uVar19 < uVar14) {
              iVar13 = iVar16 * 8;
              iVar12 = (*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(iVar16 * 2 + 2) * sizeof(uint32_t)));
              iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 8)));
              if (iVar3 - (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(iVar16 * 2 + 1) * sizeof(uint32_t))) + 8))) < 1) goto LAB_0027c610;
              iVar4 = (*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(iVar16) * sizeof(uint32_t)));
              if (iVar3 - (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 8))) < 1) goto LAB_0027c610;
              (*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(iVar16) * sizeof(uint32_t))) = iVar12;
              (*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(iVar16 * 2 + 2) * sizeof(uint32_t))) = iVar4;
            }
            else {
              iVar13 = iVar16 << 3;
LAB_0027c610:
              piVar11 = (uint32_t)((int)puVar20 + iVar13 + 4);
              iVar13 = (*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(iVar16) * sizeof(uint32_t)));
              iVar12 = (*(int32_t*)rtguest::G((uint32_t)piVar11));
              if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 8))) - (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 8))) < 1) break;
              (*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(iVar16) * sizeof(uint32_t))) = iVar12;
              (*(int32_t*)rtguest::G((uint32_t)piVar11)) = iVar13;
              uVar19 = (uint32_t)(iVar15 + 1);
            }
            iVar15 = (int)uVar19 * 2;
            iVar16 = iVar15 + 2;
            uVar7 = uVar19;
          } while ((uint32_t)(long)(iVar15 + 1) < uVar14);
        }
      } while (1 < uVar14);
    }
    fde_merge(param_1,piVar23);
    free((uint32_t)(*(int32_t*)rtguest::G((uint32_t)piVar23)));
    lVar10 = (long)(*(int32_t*)rtguest::G((uint32_t)piVar9));
  }
}


RT_GUEST_FN(0x0027c300, guest::end_fde_sort)

}  // namespace guest
