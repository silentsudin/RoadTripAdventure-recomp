// guest:0x0027ded0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _malloc_r(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
uint32_t puVar5;
  uint uVar6;
uint32_t puVar7;
uint32_t puVar8;
  uint uVar9;
  ulong uVar10;
uint32_t puVar11;
uint32_t ppuVar12;
  ulong uVar13;
  uint uVar14;
uint32_t puVar15;
uint32_t puVar16;
  uint uVar17;
  
  if (param_2 + 0x13U < 0x1f) {
    uVar17 = 0x10;
  }
  else {
    uVar17 = param_2 + 0x13U & 0xfffffff0;
  }
  __malloc_lock(param_1);
  puVar8 = (*(uint32_t*)rtguest::G(0x002e8cd8));
  uVar3 = uVar17 >> 9;
  if (uVar17 < 0x1f8) {
    puVar16 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x2e8cd4u + uVar17)));
    if (puVar16 != (uint32_t)(0x002e8cc8 + uVar17)) {
      uVar17 = (*(uint32_t*)rtguest::G((uint32_t)puVar16 + (uint32_t)(1) * sizeof(uint32_t)));
      iVar2 = (*(uint32_t*)rtguest::G((uint32_t)puVar16 + (uint32_t)(3) * sizeof(uint32_t)));
      iVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar16 + (uint32_t)(2) * sizeof(uint32_t)));
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar16 + (uVar17 & 0xfffffffc) + 4)));
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0xc))) = iVar2;
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 8))) = iVar1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar16 + (uVar17 & 0xfffffffc) + 4))) = uVar3 | 1;
      goto LAB_0027e5cc;
    }
    uVar14 = (uVar17 >> 3) + 2;
  }
  else {
    uVar14 = uVar17 >> 3;
    if (uVar3 != 0) {
      if (uVar3 < 5) {
        uVar14 = (uVar17 >> 6) + 0x38;
      }
      else {
        uVar14 = uVar3 + 0x5b;
        if (0x14 < uVar3) {
          if (uVar3 < 0x55) {
            uVar14 = (uVar17 >> 0xc) + 0x6e;
          }
          else if (uVar3 < 0x155) {
            uVar14 = (uVar17 >> 0xf) + 0x77;
          }
          else if (uVar3 < 0x555) {
            uVar14 = (uVar17 >> 0x12) + 0x7c;
          }
          else {
            uVar14 = 0x7e;
          }
        }
      }
    }
    for (puVar16 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(0x2e8cd4u + (uint32_t)(uVar14 * 2) * 4)));
        puVar16 != (uint32_t)(0x002e8cc8 + uVar14 * 8); puVar16 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar16 + (uint32_t)(3) * sizeof(uint32_t)))) {
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)puVar16 + (uint32_t)(1) * sizeof(uint32_t))) & 0xfffffffc;
      uVar13 = -(uint32_t)(uVar17 - uVar3);
      if (uVar17 <= uVar3) {
        uVar13 = (uint32_t)(uVar3 - uVar17);
      }
      if (0xf < (long)uVar13) goto LAB_0027e054;
      if (-1 < (long)uVar13) {
        iVar2 = (*(uint32_t*)rtguest::G((uint32_t)puVar16 + (uint32_t)(3) * sizeof(uint32_t)));
        iVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar16 + (uint32_t)(2) * sizeof(uint32_t)));
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar16 + uVar3 + 4))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar16 + uVar3 + 4))) | 1;
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0xc))) = iVar2;
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 8))) = iVar1;
        goto LAB_0027e5cc;
      }
    }
    uVar14 = uVar14 + 1;
  }
LAB_0027e054:
  if ((uint32_t)(*(uint32_t*)rtguest::G(0x002e8cd8)) != 0x2e8cd0u) {
    uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x002e8cd8)) + 4)));
    uVar9 = uVar3 & 0xfffffffc;
    if (uVar9 < uVar17) {
      uVar13 = -(uint32_t)(uVar17 - uVar9);
    }
    else {
      uVar13 = (uint32_t)(uVar9 - uVar17);
    }
    puVar16 = (uint32_t)puVar8;
    if (0xf < (long)uVar13) {
      ppuVar12 = (uint32_t)((*(uint32_t*)rtguest::G(0x002e8cd8)) + uVar17);
      uVar3 = (uint32_t)uVar13;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x002e8cd8)) + 4))) = uVar17 | 1;
      (*(uint32_t*)rtguest::G(0x002e8cd8)) = (uint32_t)ppuVar12;
      (*(uint32_t*)rtguest::G(0x2e8cdc)) = ppuVar12;
      (*(uint32_t*)rtguest::G((uint32_t)ppuVar12 + (uint32_t)(1) * sizeof(uint32_t))) = (uint32_t)(uVar3 | 1);
      (*(uint32_t*)rtguest::G((uint32_t)ppuVar12 + (uint32_t)(2) * sizeof(uint32_t))) = (uint32_t)0x2e8cd0u;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)ppuVar12 + uVar3))) = uVar3;
      (*(uint32_t*)rtguest::G((uint32_t)ppuVar12 + (uint32_t)(3) * sizeof(uint32_t))) = (uint32_t)0x2e8cd0u;
      goto LAB_0027e5cc;
    }
    (*(uint32_t*)rtguest::G(0x2e8cdc)) = 0x2e8cd0u;
    (*(uint32_t*)rtguest::G(0x002e8cd8)) = (uint32_t)0x2e8cd0u;
    if (-1 < (long)uVar13) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar8 + uVar9 + 4))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar8 + uVar9 + 4))) | 1;
      goto LAB_0027e5cc;
    }
    uVar4 = uVar3 >> 9;
    if (uVar9 < 0x200) {
      puVar15 = 0x002e8cc8 + (uVar3 & 0xfffffff8);
      puVar11 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x2e8cd0u + (uVar3 & 0xfffffff8))));
      (*(uint32_t*)rtguest::G(0x2e8ccc)) = (*(uint32_t*)rtguest::G(0x2e8ccc)) | (uint32_t)(1L << (long)(int)(uVar3 >> 5));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar8 + 0xc))) = puVar15;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar8 + 8))) = puVar11;
    }
    else {
      uVar6 = uVar3 >> 3;
      if (uVar4 != 0) {
        if (uVar4 < 5) {
          uVar6 = (uVar3 >> 6) + 0x38;
        }
        else {
          uVar6 = uVar4 + 0x5b;
          if (0x14 < uVar4) {
            if (uVar4 < 0x55) {
              uVar6 = (uVar3 >> 0xc) + 0x6e;
            }
            else if (uVar4 < 0x155) {
              uVar6 = (uVar3 >> 0xf) + 0x77;
            }
            else if (uVar4 < 0x555) {
              uVar6 = (uVar3 >> 0x12) + 0x7c;
            }
            else {
              uVar6 = 0x7e;
            }
          }
        }
      }
      puVar15 = 0x002e8cc8 + uVar6 * 8;
      puVar11 = (*(uint32_t*)rtguest::G((uint32_t)(0x2e8cd0u + (uint32_t)(uVar6 * 2) * 4)));
      if (puVar11 == puVar15) {
        (*(uint32_t*)rtguest::G(0x2e8ccc)) = (*(uint32_t*)rtguest::G(0x2e8ccc)) | (uint32_t)(1L << (long)((int)uVar6 >> 2));
      }
      else {
        uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar11 + 4)));
        while (uVar9 < (uVar3 & 0xfffffffc)) {
          puVar11 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar11 + 8)));
          if (puVar11 == puVar15) {
            puVar15 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar11 + 0xc)));
            goto LAB_0027e250;
          }
          uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar11 + 4)));
        }
        puVar15 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar11 + 0xc)));
      }
LAB_0027e250:
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar8 + 0xc))) = puVar15;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar8 + 8))) = puVar11;
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar15 + 8))) = puVar8;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar11 + 0xc))) = puVar8;
  }
  uVar10 = (uint32_t)(*(uint32_t*)rtguest::G(0x2e8ccc));
  uVar13 = 1L << (long)((int)uVar14 >> 2);
  if (uVar13 <= uVar10) {
    if ((uVar13 & uVar10) == 0) {
      uVar14 = uVar14 & 0xfffffffc;
      do {
        uVar13 = uVar13 << 1;
        uVar14 = uVar14 + 4;
      } while ((uVar13 & uVar10) == 0);
    }
    iVar2 = uVar14 << 3;
    do {
      puVar16 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x2e8cd4u + iVar2)));
      puVar7 = (uint32_t)(0x002e8cc8 + iVar2);
      uVar3 = uVar14;
      while( true ) {
        if (puVar16 != puVar7) {
          uVar9 = (*(uint32_t*)rtguest::G((uint32_t)puVar16 + (uint32_t)(1) * sizeof(uint32_t)));
          while( true ) {
            uVar9 = uVar9 & 0xfffffffc;
            uVar10 = -(uint32_t)(uVar17 - uVar9);
            if (uVar17 <= uVar9) {
              uVar10 = (uint32_t)(uVar9 - uVar17);
            }
            if (0xf < (long)uVar10) {
              iVar2 = (*(uint32_t*)rtguest::G((uint32_t)puVar16 + (uint32_t)(3) * sizeof(uint32_t)));
              ppuVar12 = (uint32_t)((int)puVar16 + uVar17);
              uVar3 = (uint32_t)uVar10;
              iVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar16 + (uint32_t)(2) * sizeof(uint32_t)));
              (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0xc))) = iVar2;
              (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 8))) = iVar1;
              (*(uint32_t*)rtguest::G((uint32_t)puVar16 + (uint32_t)(1) * sizeof(uint32_t))) = uVar17 | 1;
              (*(uint32_t*)rtguest::G(0x002e8cd8)) = (uint32_t)ppuVar12;
              (*(uint32_t*)rtguest::G(0x2e8cdc)) = ppuVar12;
              (*(uint32_t*)rtguest::G((uint32_t)ppuVar12 + (uint32_t)(1) * sizeof(uint32_t))) = (uint32_t)(uVar3 | 1);
              (*(uint32_t*)rtguest::G((uint32_t)ppuVar12 + (uint32_t)(2) * sizeof(uint32_t))) = (uint32_t)0x2e8cd0u;
              (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)ppuVar12 + uVar3))) = uVar3;
              (*(uint32_t*)rtguest::G((uint32_t)ppuVar12 + (uint32_t)(3) * sizeof(uint32_t))) = (uint32_t)0x2e8cd0u;
              goto LAB_0027e5cc;
            }
            if (-1 < (long)uVar10) {
              iVar2 = (*(uint32_t*)rtguest::G((uint32_t)puVar16 + (uint32_t)(3) * sizeof(uint32_t)));
              iVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar16 + (uint32_t)(2) * sizeof(uint32_t)));
              (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar16 + uVar9 + 4))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar16 + uVar9 + 4))) | 1;
              (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0xc))) = iVar2;
              (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 8))) = iVar1;
              goto LAB_0027e5cc;
            }
            puVar16 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar16 + (uint32_t)(3) * sizeof(uint32_t)));
            if (puVar16 == puVar7) break;
            uVar9 = (*(uint32_t*)rtguest::G((uint32_t)puVar16 + (uint32_t)(1) * sizeof(uint32_t)));
          }
        }
        puVar16 = puVar7 + 2;
        if ((int)uVar3 < 0x3f) {
          uVar3 = uVar3 + 1;
          puVar16 = puVar7 + 4;
        }
        puVar7 = puVar16;
        uVar3 = uVar3 + 1;
        if ((uVar3 & 3) == 0) break;
        puVar16 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar7 + (uint32_t)(3) * sizeof(uint32_t)));
      }
      puVar16 = (uint32_t)(0x002e8cc8 + iVar2);
      do {
        uVar9 = uVar14 & 3;
        uVar14 = uVar14 - 1;
        if (uVar9 == 0) {
          (*(uint32_t*)rtguest::G(0x2e8ccc)) = (*(uint32_t*)rtguest::G(0x2e8ccc)) & ~(uint32_t)uVar13;
          break;
        }
        puVar5 = puVar16 + -2;
        puVar7 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar16));
        puVar16 = puVar5;
      } while (puVar7 == puVar5);
      uVar13 = uVar13 << 1;
      if (((*(uint32_t*)rtguest::G(0x2e8ccc)) < uVar13) || (uVar13 == 0)) break;
      iVar2 = uVar3 * 8;
      uVar14 = uVar3;
      if ((uVar13 & (*(uint32_t*)rtguest::G(0x2e8ccc))) == 0) {
        do {
          uVar13 = uVar13 << 1;
          uVar14 = uVar14 + 4;
        } while ((uVar13 & (*(uint32_t*)rtguest::G(0x2e8ccc))) == 0);
        iVar2 = uVar14 * 8;
      }
    } while( true );
  }
  uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x002e8cd0)) + 4))) & 0xfffffffc;
  if (uVar3 < uVar17) {
    uVar13 = -(uint32_t)(uVar17 - uVar3);
  }
  else {
    uVar13 = (uint32_t)(uVar3 - uVar17);
  }
  if ((((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x002e8cd0)) + 4))) & 0xfffffffc) < uVar17) || ((long)uVar13 < 0x10)) {
    malloc_extend_top(param_1,uVar17);
    uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x002e8cd0)) + 4))) & 0xfffffffc;
    if (uVar3 < uVar17) {
      uVar13 = -(uint32_t)(uVar17 - uVar3);
    }
    else {
      uVar13 = (uint32_t)(uVar3 - uVar17);
    }
    if ((((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x002e8cd0)) + 4))) & 0xfffffffc) < uVar17) || ((long)uVar13 < 0x10)) {
      __malloc_unlock(param_1);
      return (uint32_t)0x0;
    }
  }
  puVar16 = (uint32_t)(*(uint32_t*)rtguest::G(0x002e8cd0));
  puVar8 = (*(uint32_t*)rtguest::G(0x002e8cd0)) + uVar17;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x002e8cd0)) + 4))) = uVar17 | 1;
  (*(uint32_t*)rtguest::G(0x002e8cd0)) = puVar8;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar8 + 4))) = (uint32_t)uVar13 | 1;
LAB_0027e5cc:
  __malloc_unlock(param_1);
  return puVar16 + 2;
}


RT_GUEST_FN(0x0027ded0, guest::_malloc_r)

}  // namespace guest
