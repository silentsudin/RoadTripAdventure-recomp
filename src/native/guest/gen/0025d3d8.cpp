// guest:0x0025d3d8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawFieldProp26Pickup(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  long lVar1;
  float fVar2;
uint32_t auStack_30 = rtguest::allocFrame(16);
  
  (*(uint64_t*)rtguest::G(0x177ea40)) = 0x200696c5dd30b476;
  lVar1 = GetFlaggedSlotCount(0xf,0x23,0);
  if ((lVar1 == 0) && (lVar1 = TestStateBitFc78(9), lVar1 == 0)) {
    vu0SubVector(auStack_30,0x2c4230,0x177ace0);
    fVar2 = rtguest::fbits(vecDotProductVu0(auStack_30,auStack_30));
    if (SQRT(fVar2) < 1.5) {
      PlayUiSound(0x302);
      SetFlaggedSlotBit(0xf,0x23,0);
    }
    vu0UnitMatrix(0x177eb90);
    tileSlotOrigin(0x177ebc0,0x2c4230,2);
  }
  return 0;
}


RT_GUEST_FN(0x0025d3d8, guest::drawFieldProp26Pickup)

}  // namespace guest
