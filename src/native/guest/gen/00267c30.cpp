// guest:0x00267c30 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t applySlotMaskDefaults(uint32_t param_1, uint64_t param_2, ...) {
  uint uVar1;
  bool bVar2;
uint32_t puVar3;
uint32_t puVar4;
uint32_t puVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
uint32_t puVar9;
  
  if (param_2 == 0) {
    puVar9 = 0x01780338;
    puVar3 = 0x002dc7ac;
    iVar7 = 0;
    iVar8 = 0;
    do {
      bVar2 = false;
      if ((*(uint32_t*)rtguest::G((uint32_t)puVar3)) != 0xffffffff) {
        bVar2 = ((*(uint32_t*)rtguest::G((uint32_t)puVar9)) & 1 << ((*(uint32_t*)rtguest::G((uint32_t)puVar3)) & 0x1f)) == 0;
      }
      if (bVar2) {
        uVar6 = 0;
        puVar5 = (uint32_t)(0x002dc460 + (iVar8 + iVar7) * 4);
        puVar4 = param_1;
        do {
          uVar6 = uVar6 + 1;
          uVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar5));
          puVar5 = puVar5 + sizeof(uint32_t);
          (*(uint32_t*)rtguest::G((uint32_t)puVar4)) = (*(uint32_t*)rtguest::G((uint32_t)puVar4)) | uVar1;
          puVar4 = puVar4 + sizeof(uint32_t);
        } while (uVar6 < 5);
      }
      iVar7 = iVar7 + 1;
      iVar8 = iVar8 + 4;
      puVar9 = puVar9 + sizeof(uint32_t);
      puVar3 = puVar3 + 2 * sizeof(uint32_t);
    } while (iVar7 < 9);
  }
  else {
    puVar9 = 0x01780338;
    puVar3 = 0x002dc7a8;
    iVar7 = 0;
    iVar8 = 0;
    do {
      bVar2 = false;
      if ((*(uint32_t*)rtguest::G((uint32_t)puVar3)) != 0xffffffff) {
        bVar2 = ((*(uint32_t*)rtguest::G((uint32_t)puVar9)) & 1 << ((*(uint32_t*)rtguest::G((uint32_t)puVar3)) & 0x1f)) == 0;
      }
      if (bVar2) {
        (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1)) | (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002dc528 + ((int)param_2 + iVar8) * 4)));
      }
      iVar7 = iVar7 + 1;
      iVar8 = iVar8 + 0x10;
      puVar9 = puVar9 + sizeof(uint32_t);
      puVar3 = puVar3 + 2 * sizeof(uint32_t);
    } while (iVar7 < 9);
  }
  if (0xe < param_2) {
    return 0;
  }
                    /* WARNING: Could not recover jumptable at 0x00267da8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  rtCall((*(uint32_t*)rtguest::G((uint32_t)(0x32f2b0u + (uint32_t)((int)param_2) * sizeof(uint32_t)))));
  return 0;
}


RT_GUEST_FN(0x00267c30, guest::applySlotMaskDefaults)

}  // namespace guest
