// guest:0x0025ca08 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawFieldProp05Pickup(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  long lVar1;
  float fVar2;
uint32_t auStack_70 = rtguest::allocFrame(48);
uint32_t auStack_40 = rtguest::allocFrame(16);
uint32_t auStack_30 = rtguest::allocFrame(16);
  
  vifFixedRegs(0x200612e5dd30b057,7,0,0xf);
  lVar1 = GetFlaggedSlotCount(0xf,0x28,0);
  if ((lVar1 == 0) && (lVar1 = TestStateBitFc78(0x27), lVar1 == 0)) {
    vu0SubVector(auStack_30,0x2c41f0,0x177ace0);
    fVar2 = rtguest::fbits(vecDotProductVu0(auStack_30,auStack_30));
    if (SQRT(fVar2) < 1.5) {
      PlayUiSound(0x302);
      SetFlaggedSlotBit(0xf,0x28,0);
    }
    vu0UnitMatrix(auStack_70);
    tileSlotOrigin(auStack_40,0x2c41f0,1);
    vifObjectMatrix(auStack_70);
    vifCallModel((*(int32_t*)rtguest::G(0x177f598)) + 0x10);
  }
  lVar1 = GetFlaggedSlotCount(0xf,0x20,0);
  if ((lVar1 == 0) && (lVar1 = TestStateBitFc78(9), lVar1 == 0)) {
    vu0SubVector(auStack_30,0x2c4200,0x177ace0);
    fVar2 = rtguest::fbits(vecDotProductVu0(auStack_30,auStack_30));
    if (SQRT(fVar2) < 1.5) {
      PlayUiSound(0x302);
      SetFlaggedSlotBit(0xf,0x20,0);
    }
    vu0UnitMatrix(auStack_70);
    tileSlotOrigin(auStack_40,0x2c4200,1);
    vifObjectMatrix(auStack_70);
    vifCallModel((*(uint32_t*)rtguest::G(0x177f59c)) + 0x10);
  }
  return 0;
}


RT_GUEST_FN(0x0025ca08, guest::drawFieldProp05Pickup)

}  // namespace guest
