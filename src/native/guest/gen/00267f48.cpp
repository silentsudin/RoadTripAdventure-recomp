// guest:0x00267f48 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t findFlaggedSlotIndexAlt(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
    uint32_t uStack_50 = rtguest::allocFrame(4);
    uint32_t uStack_58 = rtguest::allocFrame(8);
    uint32_t uStack_60 = rtguest::allocFrame(8);
  uint uVar1;
  int iVar2;
  long lVar3;
  uint uVar4;
uint32_t puVar5;
  uint uVar6;
  uint uVar7;
uint32_t puVar8;
  
  puVar8 = (uint32_t)uStack_60;
  if (param_2 == 0) {
    iVar2 = (int)param_1 * 0x14;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x002dc460 + iVar2)));
    (*(uint64_t*)rtguest::G((uint32_t)uStack_58)) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x002dc468 + iVar2)));
    (*(uint32_t*)rtguest::G((uint32_t)uStack_50)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002dc470 + iVar2)));
    if (param_1 == 9) {
      applySlotMaskDefaults(uStack_60);
    }
    iVar2 = 0;
    while( true ) {
      lVar3 = FindFlaggedSlotIndex(0,0,iVar2,0);
      puVar5 = (uint32_t)((int)uStack_60 + ((uint32_t)lVar3 >> 5) * 4);
      if (lVar3 == -1) break;
      (*(uint32_t*)rtguest::G((uint32_t)puVar5)) = (*(uint32_t*)rtguest::G((uint32_t)puVar5)) & ~(1 << ((uint32_t)lVar3 & 0x1f));
      iVar2 = iVar2 + 1;
    }
    if (param_3 == 0xffffffff) {
      uVar1 = CountSetBitsInByteArray(uStack_60,0x14);
    }
    else {
      uVar1 = 0xffffffff;
      if (param_3 < 0x280) {
        uVar1 = 0;
        uVar4 = 0;
        do {
          uVar6 = 0;
          uVar7 = (*(uint32_t*)rtguest::G((uint32_t)puVar8));
          do {
            if ((uVar7 & 1) != 0) {
              if (param_3 == uVar1) {
                return uVar4 * 0x20 + uVar6;
              }
              uVar1 = uVar1 + 1;
            }
            uVar6 = uVar6 + 1;
            uVar7 = (int)(*(uint32_t*)rtguest::G((uint32_t)puVar8)) >> (uVar6 & 0x1f);
          } while (uVar6 < 0x20);
          uVar4 = uVar4 + 1;
          puVar8 = puVar8 + 1;
        } while (uVar4 < 5);
        uVar1 = 0xffffffff;
      }
    }
  }
  else {
    (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_60 + 4)),
                         (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002dc528 + ((int)param_1 * 0x10 + (int)param_2) * 4))));
    if (param_1 == 9) {
      applySlotMaskDefaults(uStack_60);
    }
    if (param_3 == 0xffffffff) {
      uVar1 = CountSetBitsInByteArray(uStack_60,4);
    }
    else {
      uVar1 = 0xffffffff;
      if (param_3 < 0x80) {
        uVar7 = 0;
        uVar4 = 0;
        uVar1 = (uint32_t)(*(uint64_t*)rtguest::G((uint32_t)uStack_60));
        do {
          if ((uVar1 & 1) != 0) {
            if (param_3 == uVar7) {
            }
            uVar7 = uVar7 + 1;
          }
          uVar4 = uVar4 + 1;
          uVar1 = (int)(uint32_t)(*(uint64_t*)rtguest::G((uint32_t)uStack_60)) >> (uVar4 & 0x1f);
        } while (uVar4 < 0x20);
        uVar1 = 0xffffffff;
      }
    }
  }
}


RT_GUEST_FN(0x00267f48, guest::findFlaggedSlotIndexAlt)

}  // namespace guest
