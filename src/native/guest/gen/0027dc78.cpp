// guest:0x0027dc78 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t malloc_extend_top(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(96);
uint32_t puVar1;
uint32_t puVar2;
uint32_t puVar3;
  long lVar4;
  ulong uVar5;
uint32_t puVar6;
  int iVar7;
  uint uVar8;
  uint uVar9;
  
  puVar1 = (*(uint32_t*)rtguest::G(0x002e8cd0));
  param_2 = param_2 + (int)(*(uint32_t*)rtguest::G(0x2e90d8));
  uVar9 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x002e8cd0)) + 4))) & 0xfffffffc;
  uVar8 = param_2 + 0x10;
  puVar6 = (*(uint32_t*)rtguest::G(0x002e8cd0)) + uVar9;
  if ((*(uint32_t*)rtguest::G(0x2e90e0)) != (uint32_t)0xffffffff) {
    uVar8 = param_2 + 0x100fU & 0xfffff000;
  }
  puVar3 = (uint32_t)RT_CALLRA(0x27dd14, guest::_sbrk_r(param_1,uVar8));
  if ((puVar3 != (uint32_t)0xffffffff) && ((puVar6 <= puVar3 || (puVar1 == 0x002e8cc8)))) {
    (*(uint32_t*)rtguest::G(0x2e90f8)) = (*(uint32_t*)rtguest::G(0x2e90f8)) + uVar8;
    if (puVar3 == puVar6) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x002e8cd0)) + 4))) = uVar8 + uVar9 | 1;
    }
    else {
      puVar2 = puVar3;
      if ((*(uint32_t*)rtguest::G(0x2e90e0)) != (uint32_t)0xffffffff) {
        (*(uint32_t*)rtguest::G(0x2e90f8)) = puVar3 + ((int)(*(uint32_t*)rtguest::G(0x2e90f8)) - (int)puVar6);
        puVar2 = (*(uint32_t*)rtguest::G(0x2e90e0));
      }
      (*(uint32_t*)rtguest::G(0x2e90e0)) = puVar2;
      if (((uint32_t)(puVar3 + 8) & 0xf) == 0) {
        iVar7 = 0;
      }
      else {
        iVar7 = 0x10 - ((uint32_t)(puVar3 + 8) & 0xf);
        puVar3 = puVar3 + iVar7;
      }
      iVar7 = iVar7 + (0x1000 - ((uint32_t)(puVar3 + uVar8) & 0xfff));
      lVar4 = RT_CALLRA(0x27ddcc, guest::_sbrk_r((int)param_1,iVar7));
      if (lVar4 == -1) {
        return 0;
      }
      (*(uint32_t*)rtguest::G(0x2e90f8)) = (*(uint32_t*)rtguest::G(0x2e90f8)) + iVar7;
      (*(uint32_t*)rtguest::G(0x002e8cd0)) = puVar3;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 4))) = ((int)lVar4 - (int)puVar3) + iVar7 | 1;
      if (puVar1 != 0x002e8cc8) {
        if (uVar9 < 0x10) {
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 4))) = 1;
          return 0;
        }
        uVar9 = uVar9 - 0xc & 0xfffffff0;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 4))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 4))) & 1 | uVar9;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + uVar9 + 8))) = 5;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + uVar9 + 4))) = 5;
        if (0xf < uVar9) {
          RT_CALLRA(0x27de64, guest::_free_r((int)param_1,puVar1 + 8));
        }
      }
    }
    uVar5 = (uint32_t)(int)(*(uint32_t*)rtguest::G(0x2e90f8));
    if ((*(uint32_t*)rtguest::G(0x2e90e8)) < uVar5) {
      (*(uint32_t*)rtguest::G(0x2e90e8)) = uVar5;
    }
    if ((*(uint32_t*)rtguest::G(0x2e90f0)) < uVar5) {
      (*(uint32_t*)rtguest::G(0x2e90f0)) = uVar5;
    }
  }
  return 0;
}


RT_GUEST_FN(0x0027dc78, guest::malloc_extend_top)

}  // namespace guest
