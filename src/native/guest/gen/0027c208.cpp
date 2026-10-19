// guest:0x0027c208 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t fde_merge(uint32_t param_1, uint32_t param_2, ...) {
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
uint32_t puVar7;
  undefined4 uVar8;
uint32_t piVar9;
  int iVar10;
uint32_t puVar11;
  int iVar12;
  int iVar13;
uint32_t piVar14;
  int iVar15;
  
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t)));
  if (iVar1 != 0) {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t)));
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_1));
    iVar6 = iVar2 * 4;
    piVar9 = (uint32_t)(iVar1 * 4 + (*(int32_t*)rtguest::G((uint32_t)param_2)));
    piVar14 = (uint32_t)(iVar6 + iVar3 + -4);
    iVar15 = iVar1;
    iVar13 = iVar2;
    do {
      piVar9 = piVar9 + -1;
      iVar15 = iVar15 + -1;
      iVar4 = (*(int32_t*)rtguest::G((uint32_t)piVar9));
      if ((iVar13 != 0) && (iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 8))), 0 < (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)piVar14)) + 8))) - iVar5)) {
        iVar12 = iVar6 + iVar15 * 4;
        puVar11 = (uint32_t)(iVar6 + iVar3 + -4);
        uVar8 = (*(uint32_t*)rtguest::G((uint32_t)puVar11));
        iVar10 = iVar6;
        while( true ) {
          puVar11 = puVar11 + -1;
          puVar7 = (uint32_t)(iVar12 + iVar3);
          iVar12 = iVar12 + -4;
          iVar10 = iVar10 + -4;
          iVar13 = iVar13 + -1;
          (*(uint32_t*)rtguest::G((uint32_t)puVar7)) = uVar8;
          piVar14 = piVar14 + -1;
          iVar6 = iVar6 + -4;
          if ((iVar13 == 0) || ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + iVar10 + -4))) + 8))) - iVar5 < 1)) break;
          uVar8 = (*(uint32_t*)rtguest::G((uint32_t)puVar11));
        }
      }
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((iVar13 + iVar15) * 4 + iVar3))) = iVar4;
    } while (iVar15 != 0);
    (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t))) = iVar2 + iVar1;
  }
  return 0;
}


RT_GUEST_FN(0x0027c208, guest::fde_merge)

}  // namespace guest
