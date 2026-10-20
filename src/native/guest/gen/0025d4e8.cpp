// guest:0x0025d4e8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawFieldProp21Pickup(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(48);
  long lVar1;
  float fVar2;
uint32_t auStack_30 = rtguest::allocFrame(16);
  
  (*(uint64_t*)rtguest::G(0x177ea38)) = 0x200612e5dd30b057;
  lVar1 = GetFlaggedSlotCount(0xf,0x21,0);
  if ((lVar1 == 0) && (lVar1 = TestStateBitFc78(9), lVar1 == 0)) {
    vu0SubVector(auStack_30,0x2c4240,0x177ace0);
    fVar2 = rtguest::fbits(vecDotProductVu0(auStack_30,auStack_30));
    if (SQRT(fVar2) < 1.5) {
      PlayUiSound(0x302);
      SetFlaggedSlotBit(0xf,0x21,0);
    }
    vu0UnitMatrix(0x177eb50);
    tileSlotOrigin(0x177eb80,0x2c4240,1);
  }
  return 0;
}


RT_GUEST_FN(0x0025d4e8, guest::drawFieldProp21Pickup)

}  // namespace guest
