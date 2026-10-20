// guest:0x0025c300 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawFieldProp23Pickup(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  long lVar1;
  float fVar2;
uint32_t auStack_80 = rtguest::allocFrame(48);
uint32_t auStack_50 = rtguest::allocFrame(16);
uint32_t auStack_40 = rtguest::allocFrame(16);
  
  vifFixedRegs(0x20079f05dd30bcb8,7,0,0xf);
  lVar1 = GetFlaggedSlotCount(0xf,(*(uint32_t*)rtguest::G(0x2c4190)),0);
  if ((lVar1 == 0) && (lVar1 = TestStateBitFc78(9), lVar1 == 0)) {
    vu0SubVector(auStack_40,0x2c4170,0x177ace0);
    fVar2 = rtguest::fbits(vecDotProductVu0(auStack_40,auStack_40));
    if (SQRT(fVar2) < 1.5) {
      PlayUiSound(0x302);
      SetFlaggedSlotBit(0xf,(*(uint32_t*)rtguest::G(0x2c4190)),0);
    }
    vu0UnitMatrix(auStack_80);
    tileSlotOrigin(auStack_50,0x2c4170,3);
    vifObjectMatrix(auStack_80);
    vifCallModel((*(int32_t*)rtguest::G(0x177f5c8)) + 0x10);
  }
  lVar1 = GetFlaggedSlotCount(0xf,(*(uint32_t*)rtguest::G(0x2c4194)),0);
  if ((lVar1 == 0) && (lVar1 = TestStateBitFc78(9), lVar1 == 0)) {
    vu0SubVector(auStack_40,0x2c4180,0x177ace0);
    fVar2 = rtguest::fbits(vecDotProductVu0(auStack_40,auStack_40));
    if (SQRT(fVar2) < 2.0) {
      PlayUiSound(0x302);
      SetFlaggedSlotBit(0xf,(*(uint32_t*)rtguest::G(0x2c4194)),0);
    }
    vu0UnitMatrix(auStack_80);
    tileSlotOrigin(auStack_50,0x2c4180,3);
    vifObjectMatrix(auStack_80);
    vifCallModel((*(uint32_t*)rtguest::G(0x177f5cc)) + 0x10);
  }
  return 0;
}


RT_GUEST_FN(0x0025c300, guest::drawFieldProp23Pickup)

}  // namespace guest
