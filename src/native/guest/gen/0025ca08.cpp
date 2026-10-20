// guest:0x0025ca08 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawFieldProp05Pickup(uint32_t q0, ...) {
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
  
  RT_CALLRA(0x25ca38, guest::vifFixedRegs(0x200612e5dd30b057,7,0,0xf));
  lVar1 = RT_CALLRA(0x25ca48, guest::GetFlaggedSlotCount(0xf,0x28,0));
  if ((lVar1 == 0) && (lVar1 = TestStateBitFc78(0x27), lVar1 == 0)) {
    RT_CALLRA(0x25ca80, guest::vu0SubVector(auStack_30,0x2c41f0,0x177ace0));
    fVar2 = rtguest::fbits(RT_CALLRA(0x25ca8c, guest::vecDotProductVu0(auStack_30,auStack_30)));
    if (SQRT(fVar2) < 1.5) {
      RT_CALLRA(0x25cad8, guest::PlayUiSound(0x302));
      RT_CALLRA(0x25cae8, guest::SetFlaggedSlotBit(0xf,0x28,0));
    }
    RT_CALLRA(0x25caf0, guest::vu0UnitMatrix(auStack_70));
    RT_CALLRA(0x25cb00, guest::tileSlotOrigin(auStack_40,0x2c41f0,1));
    RT_CALLRA(0x25cb08, guest::vifObjectMatrix(auStack_70));
    RT_CALLRA(0x25cb18, guest::vifCallModel((*(int32_t*)rtguest::G(0x177f598)) + 0x10));
  }
  lVar1 = RT_CALLRA(0x25cb28, guest::GetFlaggedSlotCount(0xf,0x20,0));
  if ((lVar1 == 0) && (lVar1 = TestStateBitFc78(9), lVar1 == 0)) {
    RT_CALLRA(0x25cb60, guest::vu0SubVector(auStack_30,0x2c4200,0x177ace0));
    fVar2 = rtguest::fbits(RT_CALLRA(0x25cb6c, guest::vecDotProductVu0(auStack_30,auStack_30)));
    if (SQRT(fVar2) < 1.5) {
      RT_CALLRA(0x25cbb8, guest::PlayUiSound(0x302));
      RT_CALLRA(0x25cbc8, guest::SetFlaggedSlotBit(0xf,0x20,0));
    }
    RT_CALLRA(0x25cbd0, guest::vu0UnitMatrix(auStack_70));
    RT_CALLRA(0x25cbe0, guest::tileSlotOrigin(auStack_40,0x2c4200,1));
    RT_CALLRA(0x25cbe8, guest::vifObjectMatrix(auStack_70));
    RT_CALLRA(0x25cbf8, guest::vifCallModel((*(uint32_t*)rtguest::G(0x177f59c)) + 0x10));
  }
  return 0;
}


RT_GUEST_FN(0x0025ca08, guest::drawFieldProp05Pickup)

}  // namespace guest
