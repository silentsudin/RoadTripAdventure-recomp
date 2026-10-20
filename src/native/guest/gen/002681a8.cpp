// guest:0x002681a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t countFlaggedSlotsAlt(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x50) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x54) = 0x00000000u;
  }
    uint32_t uStack_60 = rtguest::allocFrame(8);
    uint32_t uStack_58 = rtguest::allocFrame(8);
    uint32_t uStack_50 = rtguest::allocFrame(4);
  int iVar1;
  uint uVar2;
  long lVar3;
uint32_t puVar4;
  int iVar5;
  undefined4 uVar6;
  uint uVar7;
  uint uVar8;
uint32_t piVar9;
  
  piVar9 = (uint32_t)uStack_60;
  if (param_2 == 0) {
    iVar1 = (int)param_1 * 0x14;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x002dc460 + iVar1)));
    (*(uint64_t*)rtguest::G((uint32_t)uStack_58)) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x002dc468 + iVar1)));
    (*(uint32_t*)rtguest::G((uint32_t)uStack_50)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002dc470 + iVar1)));
    if (param_1 == 9) {
      RT_CALLRA(0x2681fc, guest::applySlotMaskDefaults(uStack_60));
    }
    iVar1 = 0;
    while( true ) {
      lVar3 = RT_CALLRA(0x26832c, guest::FindFlaggedSlotIndex(0,0,iVar1,0));
      puVar4 = (uint32_t)((int)uStack_60 + ((uint32_t)lVar3 >> 5) * 4);
      if (lVar3 == -1) break;
      (*(uint32_t*)rtguest::G((uint32_t)puVar4)) = (*(uint32_t*)rtguest::G((uint32_t)puVar4)) & ~(1 << ((uint32_t)lVar3 & 0x1f));
      iVar1 = iVar1 + 1;
    }
    if (param_3 != 0xffffffff) {
      if (0x27f < param_3) {
        return -1;
      }
      iVar1 = 0;
      uVar8 = 0;
      do {
        uVar7 = 0;
        uVar2 = uVar8 * 0x20;
        do {
          (*(uint32_t*)rtguest::G((uint32_t)uStack_60 + 0)) = (*(int32_t*)rtguest::G((uint32_t)piVar9));
          if (param_3 == uVar2) goto LAB_002683c8;
          uVar2 = uVar7 & 0x1f;
          uVar7 = uVar7 + 1;
          if (((int)(*(uint64_t*)rtguest::G((uint32_t)uStack_60)) >> uVar2 & 1U) != 0) {
            iVar1 = iVar1 + 1;
          }
          uVar2 = uVar8 * 0x20 + uVar7;
        } while (uVar7 < 0x20);
        uVar8 = uVar8 + 1;
        piVar9 = piVar9 + 0x4;  // T* element step
        if (4 < uVar8) {
          return -1;
        }
      } while( true );
    }
    uVar6 = 0x14;
  }
  else {
    (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_60 + 4)),
                         (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002dc528 + ((int)param_1 * 0x10 + (int)param_2) * 4))));
    if (param_1 == 9) {
      RT_CALLRA(0x2682f0, guest::applySlotMaskDefaults(uStack_60));
    }
    if (param_3 != 0xffffffff) {
      if (0x7f < param_3) {
        return -1;
      }
      iVar1 = 0;
      uVar7 = 0;
      while (param_3 != uVar7) {
        uVar8 = uVar7 & 0x1f;
        uVar7 = uVar7 + 1;
        if (((int)(*(uint64_t*)rtguest::G((uint32_t)uStack_60)) >> uVar8 & 1U) != 0) {
          iVar1 = iVar1 + 1;
        }
        if (0x1f < uVar7) {
          return -1;
        }
      }
LAB_002683c8:
      iVar5 = -1;
      if (((int)(*(uint64_t*)rtguest::G((uint32_t)uStack_60)) >> (uVar7 & 0x1f) & 1U) != 0) {
        iVar5 = iVar1;
      }
      return iVar5;
    }
    uVar6 = 4;
  }
  iVar1 = RT_CALLRA(0x268364, guest::CountSetBitsInByteArray(uStack_60,uVar6));
  return iVar1;
}


RT_GUEST_FN(0x002681a8, guest::countFlaggedSlotsAlt)

}  // namespace guest
