// guest:0x0025c528 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawFieldProp2fPickup(uint32_t q0, ...) {
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
  
  RT_CALLRA(0x25c558, guest::vifFixedRegs(0x20079f05dd30bcb8,7,0,0xf));
  lVar1 = RT_CALLRA(0x25c568, guest::GetFlaggedSlotCount(0xf,0x2c,0));
  if ((lVar1 == 0) && (lVar1 = TestStateBitFc78(0x54), lVar1 == 0)) {
    RT_CALLRA(0x25c5a0, guest::vu0SubVector(auStack_30,0x2a2be0,0x177ace0));
    fVar2 = rtguest::fbits(RT_CALLRA(0x25c5ac, guest::vecDotProductVu0(auStack_30,auStack_30)));
    if (SQRT(fVar2) < 2.5) {
      RT_CALLRA(0x25c5f8, guest::PlayUiSound(0x302));
      RT_CALLRA(0x25c608, guest::SetFlaggedSlotBit(0xf,0x2c,0));
    }
    RT_CALLRA(0x25c610, guest::vu0UnitMatrix(auStack_70));
    RT_CALLRA(0x25c620, guest::tileSlotOrigin(auStack_40,0x2a2be0,3));
    RT_CALLRA(0x25c628, guest::vifObjectMatrix(auStack_70));
    RT_CALLRA(0x25c638, guest::vifCallModel((*(int32_t*)rtguest::G(0x177f5c8)) + 0x10));
  }
  lVar1 = RT_CALLRA(0x25c648, guest::GetFlaggedSlotCount(0xf,0x24,0));
  if ((lVar1 == 0) && (lVar1 = TestStateBitFc78(9), lVar1 == 0)) {
    RT_CALLRA(0x25c680, guest::vu0SubVector(auStack_30,0x2c41b0,0x177ace0));
    fVar2 = rtguest::fbits(RT_CALLRA(0x25c68c, guest::vecDotProductVu0(auStack_30,auStack_30)));
    if (SQRT(fVar2) < 2.5) {
      RT_CALLRA(0x25c6d8, guest::PlayUiSound(0x302));
      RT_CALLRA(0x25c6e8, guest::SetFlaggedSlotBit(0xf,0x24,0));
    }
    RT_CALLRA(0x25c6f0, guest::vu0UnitMatrix(auStack_70));
    RT_CALLRA(0x25c700, guest::tileSlotOrigin(auStack_40,0x2c41b0,3));
    RT_CALLRA(0x25c708, guest::vifObjectMatrix(auStack_70));
    RT_CALLRA(0x25c718, guest::vifCallModel((*(uint32_t*)rtguest::G(0x177f5cc)) + 0x10));
  }
  lVar1 = RT_CALLRA(0x25c728, guest::GetFlaggedSlotCount(0xf,0x2a,0));
  if ((lVar1 == 0) && (lVar1 = TestStateBitFc78(0x40), lVar1 == 0)) {
    RT_CALLRA(0x25c760, guest::vu0SubVector(auStack_30,0x2c41c0,0x177ace0));
    fVar2 = rtguest::fbits(RT_CALLRA(0x25c76c, guest::vecDotProductVu0(auStack_30,auStack_30)));
    if (SQRT(fVar2) < 1.5) {
      RT_CALLRA(0x25c7b8, guest::PlayUiSound(0x302));
      RT_CALLRA(0x25c7c8, guest::SetFlaggedSlotBit(0xf,0x2a,0));
    }
    RT_CALLRA(0x25c7d0, guest::vu0UnitMatrix(auStack_70));
    RT_CALLRA(0x25c7e0, guest::tileSlotOrigin(auStack_40,0x2c41c0,3));
    RT_CALLRA(0x25c7e8, guest::vifObjectMatrix(auStack_70));
    RT_CALLRA(0x25c7f8, guest::vifCallModel((*(uint32_t*)rtguest::G(0x177f5d0)) + 0x10));
  }
  return 0;
}


RT_GUEST_FN(0x0025c528, guest::drawFieldProp2fPickup)

}  // namespace guest
