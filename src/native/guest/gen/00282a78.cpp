// guest:0x00282a78 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _free_r(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  int iVar1;
  int iVar2;
uint32_t ppuVar3;
  uint uVar4;
uint32_t puVar5;
  uint uVar6;
uint32_t puVar7;
  uint uVar8;
uint32_t piVar9;
  bool bVar10;
  
  if (param_2 == 0) {
    return 0;
  }
  __malloc_lock();
  piVar9 = (uint32_t)((int)param_2 + -8);
  uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + -4)));
  uVar8 = uVar4 & 0xfffffffe;
  puVar5 = (uint32_t)((int)piVar9 + uVar8);
  uVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 4))) & 0xfffffffc;
  if (puVar5 == (*(uint32_t*)rtguest::G(0x002e8cd0))) {
    uVar8 = uVar8 + uVar6;
    (*(uint32_t*)rtguest::G(0x002e8cd0)) = (uint32_t)piVar9;
    if ((uVar4 & 1) == 0) {
      (*(uint32_t*)rtguest::G(0x002e8cd0)) = (uint32_t)((int)piVar9 - (*(int32_t*)rtguest::G((uint32_t)piVar9)));
      uVar8 = uVar8 + (*(int32_t*)rtguest::G((uint32_t)piVar9));
      iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)(*(uint32_t*)rtguest::G(0x002e8cd0)) + 0xc)));
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)(*(uint32_t*)rtguest::G(0x002e8cd0)) + 8)));
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xc))) = iVar1;
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 8))) = iVar2;
    }
    bVar10 = (*(uint32_t*)rtguest::G(0x2e90d0)) <= uVar8;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)(*(uint32_t*)rtguest::G(0x002e8cd0)) + 4))) = uVar8 | 1;
    if (bVar10) {
      _malloc_trim_r(param_1,(*(uint32_t*)rtguest::G(0x2e90d8)));
    }
    __malloc_unlock(param_1);
    return 0;
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 4))) = uVar6;
  bVar10 = false;
  if ((uVar4 & 1) == 0) {
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)piVar9));
    piVar9 = (uint32_t)((int)piVar9 - iVar1);
    uVar8 = uVar8 + iVar1;
    ppuVar3 = (uint32_t)(*(int32_t*)rtguest::G((uint32_t)piVar9 + (uint32_t)(2) * sizeof(int32_t)));
    if (ppuVar3 == 0x2e8cd0u) {
      bVar10 = true;
    }
    else {
      puVar7 = (uint32_t)(*(int32_t*)rtguest::G((uint32_t)piVar9 + (uint32_t)(3) * sizeof(int32_t)));
      (*(uint32_t*)rtguest::G((uint32_t)ppuVar3 + (uint32_t)(3) * sizeof(uint32_t))) = puVar7;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar7 + 8))) = ppuVar3;
    }
  }
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + uVar6 + 4))) & 1) == 0) {
    uVar8 = uVar8 + uVar6;
    if (bVar10) {
      ppuVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 8)));
      puVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0xc)));
    }
    else {
      ppuVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 8)));
      if (ppuVar3 == 0x2e8cd0u) {
        bVar10 = true;
        (*(uint32_t*)rtguest::G(0x002e8cd8)) = (uint32_t)piVar9;
        (*(uint32_t*)rtguest::G(0x2e8cdc)) = piVar9;
        (*(int32_t*)rtguest::G((uint32_t)piVar9 + (uint32_t)(2) * sizeof(int32_t))) = (int)0x2e8cd0u;
        (*(int32_t*)rtguest::G((uint32_t)piVar9 + (uint32_t)(3) * sizeof(int32_t))) = (int)0x2e8cd0u;
        goto LAB_00282be8;
      }
      puVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0xc)));
    }
    (*(uint32_t*)rtguest::G((uint32_t)ppuVar3 + (uint32_t)(3) * sizeof(uint32_t))) = puVar5;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 8))) = ppuVar3;
  }
LAB_00282be8:
  (*(int32_t*)rtguest::G((uint32_t)piVar9 + (uint32_t)(1) * sizeof(int32_t))) = uVar8 | 1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)piVar9 + uVar8))) = uVar8;
  if (!bVar10) {
    if (uVar8 < 0x200) {
      puVar7 = 0x002e8cc8 + (uVar8 & 0xfffffff8);
      puVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x2e8cd0u + (uVar8 & 0xfffffff8))));
      (*(uint32_t*)rtguest::G(0x2e8ccc)) = (*(uint32_t*)rtguest::G(0x2e8ccc)) | (uint32_t)(1L << (long)(int)(uVar8 >> 5));
      (*(int32_t*)rtguest::G((uint32_t)piVar9 + (uint32_t)(3) * sizeof(int32_t))) = (int)puVar7;
      (*(int32_t*)rtguest::G((uint32_t)piVar9 + (uint32_t)(2) * sizeof(int32_t))) = (int)puVar5;
    }
    else {
      uVar4 = uVar8 >> 9;
      uVar6 = uVar8 >> 3;
      if (uVar4 != 0) {
        if (uVar4 < 5) {
          uVar6 = (uVar8 >> 6) + 0x38;
        }
        else {
          uVar6 = uVar4 + 0x5b;
          if (0x14 < uVar4) {
            if (uVar4 < 0x55) {
              uVar6 = (uVar8 >> 0xc) + 0x6e;
            }
            else if (uVar4 < 0x155) {
              uVar6 = (uVar8 >> 0xf) + 0x77;
            }
            else if (uVar4 < 0x555) {
              uVar6 = (uVar8 >> 0x12) + 0x7c;
            }
            else {
              uVar6 = 0x7e;
            }
          }
        }
      }
      puVar7 = 0x002e8cc8 + uVar6 * 8;
      puVar5 = (*(uint32_t*)rtguest::G((uint32_t)(0x2e8cd0u + (uint32_t)(uVar6 * 2) * 4)));
      if (puVar5 == puVar7) {
        (*(uint32_t*)rtguest::G(0x2e8ccc)) = (*(uint32_t*)rtguest::G(0x2e8ccc)) | (uint32_t)(1L << (long)((int)uVar6 >> 2));
      }
      else {
        uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 4)));
        while (uVar8 < (uVar4 & 0xfffffffc)) {
          puVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 8)));
          if (puVar5 == puVar7) {
            puVar7 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0xc)));
            goto LAB_00282d30;
          }
          uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 4)));
        }
        puVar7 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0xc)));
      }
LAB_00282d30:
      (*(int32_t*)rtguest::G((uint32_t)piVar9 + (uint32_t)(3) * sizeof(int32_t))) = (int)puVar7;
      (*(int32_t*)rtguest::G((uint32_t)piVar9 + (uint32_t)(2) * sizeof(int32_t))) = (int)puVar5;
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar7 + 8))) = piVar9;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0xc))) = piVar9;
  }
  __malloc_unlock(param_1);
  return 0;
}


RT_GUEST_FN(0x00282a78, guest::_free_r)

}  // namespace guest
