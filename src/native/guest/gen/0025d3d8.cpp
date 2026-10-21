// guest:0x0025d3d8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawFieldProp26Pickup(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x18, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
  long lVar1;
  float fVar2;
uint32_t auStack_30 = rtguest::allocFrame(16);
  
  (*(uint64_t*)rtguest::G(0x177ea40)) = 0x200696c5dd30b476;
  lVar1 = RT_CALLRA(0x25d410, guest::GetFlaggedSlotCount(0xf,0x23,0));
  if ((lVar1 == 0) && (lVar1 = TestStateBitFc78(9), lVar1 == 0)) {
    RT_CALLRA(0x25d440, guest::vu0SubVector(auStack_30,0x2c4230,0x177ace0));
    fVar2 = rtguest::fbits(RT_CALLRA(0x25d44c, guest::vecDotProductVu0(auStack_30,auStack_30)));
    if (SQRT(fVar2) < 1.5) {
      RT_CALLRA(0x25d498, guest::PlayUiSound(0x302));
      RT_CALLRA(0x25d4a8, guest::SetFlaggedSlotBit(0xf,0x23,0));
    }
    RT_CALLRA(0x25d4b8, guest::vu0UnitMatrix(0x177eb90));
    RT_CALLRA(0x25d4c8, guest::tileSlotOrigin(0x177ebc0,0x2c4230,2));
  }
  return 0;
}


RT_GUEST_FN(0x0025d3d8, guest::drawFieldProp26Pickup)

}  // namespace guest
