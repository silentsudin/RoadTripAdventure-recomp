// guest:0x0025c1d8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawFieldProp24Pickup(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(112);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x60) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x64) = 0x00000000u;
  }
  long lVar1;
  float fVar2;
uint32_t auStack_70 = rtguest::allocFrame(48);
uint32_t auStack_40 = rtguest::allocFrame(16);
uint32_t auStack_30 = rtguest::allocFrame(16);
  
  RT_CALLRA(0x25c208, guest::vifFixedRegs(0x20058f05dd30ac38,7,0,0xf));
  lVar1 = RT_CALLRA(0x25c218, guest::GetFlaggedSlotCount(0xf,0x2b,0));
  if ((lVar1 == 0) && (lVar1 = TestStateBitFc78(0x50), lVar1 == 0)) {
    RT_CALLRA(0x25c24c, guest::vu0SubVector(auStack_30,0x2c4160,0x177ace0));
    fVar2 = rtguest::fbits(RT_CALLRA(0x25c258, guest::vecDotProductVu0(auStack_30,auStack_30)));
    if (SQRT(fVar2) < 1.5) {
      RT_CALLRA(0x25c2a4, guest::PlayUiSound(0x302));
      RT_CALLRA(0x25c2b4, guest::SetFlaggedSlotBit(0xf,0x2b,0));
    }
    RT_CALLRA(0x25c2bc, guest::vu0UnitMatrix(auStack_70));
    RT_CALLRA(0x25c2cc, guest::tileSlotOrigin(auStack_40,0x2c4160,0));
    RT_CALLRA(0x25c2d4, guest::vifObjectMatrix(auStack_70));
    RT_CALLRA(0x25c2e4, guest::vifCallModel((*(uint32_t*)rtguest::G(0x177f580)) + 0x10));
  }
  return 0;
}


RT_GUEST_FN(0x0025c1d8, guest::drawFieldProp24Pickup)

}  // namespace guest
