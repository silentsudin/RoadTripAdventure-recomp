// guest:0x0025c300 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawFieldProp23Pickup(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(128);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x70) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x74) = 0x00000000u;
  }
  long lVar1;
  float fVar2;
uint32_t auStack_80 = rtguest::allocFrame(48);
uint32_t auStack_50 = rtguest::allocFrame(16);
uint32_t auStack_40 = rtguest::allocFrame(16);
  
  RT_CALLRA(0x25c340, guest::vifFixedRegs(0x20079f05dd30bcb8,7,0,0xf));
  lVar1 = RT_CALLRA(0x25c350, guest::GetFlaggedSlotCount(0xf,(*(uint32_t*)rtguest::G(0x2c4190)),0));
  if ((lVar1 == 0) && (lVar1 = TestStateBitFc78(9), lVar1 == 0)) {
    RT_CALLRA(0x25c388, guest::vu0SubVector(auStack_40,0x2c4170,0x177ace0));
    fVar2 = rtguest::fbits(RT_CALLRA(0x25c394, guest::vecDotProductVu0(auStack_40,auStack_40)));
    if (SQRT(fVar2) < 1.5) {
      RT_CALLRA(0x25c3e0, guest::PlayUiSound(0x302));
      RT_CALLRA(0x25c3f0, guest::SetFlaggedSlotBit(0xf,(*(uint32_t*)rtguest::G(0x2c4190)),0));
    }
    RT_CALLRA(0x25c3f8, guest::vu0UnitMatrix(auStack_80));
    RT_CALLRA(0x25c408, guest::tileSlotOrigin(auStack_50,0x2c4170,3));
    RT_CALLRA(0x25c410, guest::vifObjectMatrix(auStack_80));
    RT_CALLRA(0x25c420, guest::vifCallModel((*(int32_t*)rtguest::G(0x177f5c8)) + 0x10));
  }
  lVar1 = RT_CALLRA(0x25c434, guest::GetFlaggedSlotCount(0xf,(*(uint32_t*)rtguest::G(0x2c4194)),0));
  if ((lVar1 == 0) && (lVar1 = TestStateBitFc78(9), lVar1 == 0)) {
    RT_CALLRA(0x25c46c, guest::vu0SubVector(auStack_40,0x2c4180,0x177ace0));
    fVar2 = rtguest::fbits(RT_CALLRA(0x25c478, guest::vecDotProductVu0(auStack_40,auStack_40)));
    if (SQRT(fVar2) < 2.0) {
      RT_CALLRA(0x25c4c4, guest::PlayUiSound(0x302));
      RT_CALLRA(0x25c4d4, guest::SetFlaggedSlotBit(0xf,(*(uint32_t*)rtguest::G(0x2c4194)),0));
    }
    RT_CALLRA(0x25c4dc, guest::vu0UnitMatrix(auStack_80));
    RT_CALLRA(0x25c4ec, guest::tileSlotOrigin(auStack_50,0x2c4180,3));
    RT_CALLRA(0x25c4f4, guest::vifObjectMatrix(auStack_80));
    RT_CALLRA(0x25c504, guest::vifCallModel((*(uint32_t*)rtguest::G(0x177f5cc)) + 0x10));
  }
  return 0;
}


RT_GUEST_FN(0x0025c300, guest::drawFieldProp23Pickup)

}  // namespace guest
