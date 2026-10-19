// guest:0x0025c528 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawFieldProp2fPickup(uint32_t q0, ...) {
  long lVar1;
  float fVar2;
uint32_t auStack_70 = rtguest::allocFrame(48);
uint32_t auStack_40 = rtguest::allocFrame(16);
uint32_t auStack_30 = rtguest::allocFrame(16);
  
  vifFixedRegs(0x20079f05dd30bcb8,7,0,0xf);
  lVar1 = GetFlaggedSlotCount(0xf,0x2c,0);
  if ((lVar1 == 0) && (lVar1 = TestStateBitFc78(0x54), lVar1 == 0)) {
    vu0SubVector(auStack_30,0x2a2be0,0x177ace0);
    fVar2 = rtguest::fbits(vecDotProductVu0(auStack_30,auStack_30));
    if (SQRT(fVar2) < 2.5) {
      PlayUiSound(0x302);
      SetFlaggedSlotBit(0xf,0x2c,0);
    }
    vu0UnitMatrix(auStack_70);
    tileSlotOrigin(auStack_40,0x2a2be0,3);
    vifObjectMatrix(auStack_70);
    vifCallModel((*(int32_t*)rtguest::G(0x177f5c8)) + 0x10);
  }
  lVar1 = GetFlaggedSlotCount(0xf,0x24,0);
  if ((lVar1 == 0) && (lVar1 = TestStateBitFc78(9), lVar1 == 0)) {
    vu0SubVector(auStack_30,0x2c41b0,0x177ace0);
    fVar2 = rtguest::fbits(vecDotProductVu0(auStack_30,auStack_30));
    if (SQRT(fVar2) < 2.5) {
      PlayUiSound(0x302);
      SetFlaggedSlotBit(0xf,0x24,0);
    }
    vu0UnitMatrix(auStack_70);
    tileSlotOrigin(auStack_40,0x2c41b0,3);
    vifObjectMatrix(auStack_70);
    vifCallModel((*(uint32_t*)rtguest::G(0x177f5cc)) + 0x10);
  }
  lVar1 = GetFlaggedSlotCount(0xf,0x2a,0);
  if ((lVar1 == 0) && (lVar1 = TestStateBitFc78(0x40), lVar1 == 0)) {
    vu0SubVector(auStack_30,0x2c41c0,0x177ace0);
    fVar2 = rtguest::fbits(vecDotProductVu0(auStack_30,auStack_30));
    if (SQRT(fVar2) < 1.5) {
      PlayUiSound(0x302);
      SetFlaggedSlotBit(0xf,0x2a,0);
    }
    vu0UnitMatrix(auStack_70);
    tileSlotOrigin(auStack_40,0x2c41c0,3);
    vifObjectMatrix(auStack_70);
    vifCallModel((*(uint32_t*)rtguest::G(0x177f5d0)) + 0x10);
  }
  return 0;
}


RT_GUEST_FN(0x0025c528, guest::drawFieldProp2fPickup)

}  // namespace guest
