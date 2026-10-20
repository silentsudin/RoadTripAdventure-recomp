// guest:0x0025be98 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawProximityEventProp(uint32_t param_1, ...) {
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
  
  RT_CALLRA(0x25becc, guest::vifFixedRegs(0x20079f05dd30bcb8,7,0,0xf));
  if ((*(int8_t*)rtguest::G(0x33573a)) == 4) {
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) & 1) != 0) {
      RT_CALLRA(0x25bf20, guest::vu0UnitMatrix(auStack_70));
      RT_CALLRA(0x25bf34, guest::tileSlotOrigin(auStack_40,0x2a2bc0,3));
      RT_CALLRA(0x25bf3c, guest::vifObjectMatrix(auStack_70));
      RT_CALLRA(0x25bf4c, guest::vifCallModel((*(int32_t*)rtguest::G(0x177f5c8)) + 0x10));
    }
  }
  else {
    if ((*(int8_t*)rtguest::G(0x33573a)) < 5) {
      if ((*(int8_t*)rtguest::G(0x33573a)) == 0) goto LAB_0025bf98;
    }
    else if ((*(int8_t*)rtguest::G(0x33573a)) == 5) goto LAB_0025bf98;
    RT_CALLRA(0x25bf60, guest::vu0UnitMatrix(auStack_70));
    RT_CALLRA(0x25bf74, guest::tileSlotOrigin(auStack_40,0x2a2bc0,3));
    RT_CALLRA(0x25bf7c, guest::vifObjectMatrix(auStack_70));
    RT_CALLRA(0x25bf8c, guest::vifCallModel((*(int32_t*)rtguest::G(0x177f5c8)) + 0x10));
  }
LAB_0025bf98:
  lVar1 = RT_CALLRA(0x25bf9c, guest::GetFlaggedSlotCount(0xf,0x17,0));
  if ((lVar1 == 0) && (lVar1 = TestStateBitFc78(5), lVar1 == 0)) {
    RT_CALLRA(0x25bfd4, guest::vu0SubVector(auStack_30,0x2c4120,0x177ace0));
    fVar2 = rtguest::fbits(RT_CALLRA(0x25bfe0, guest::vecDotProductVu0(auStack_30,auStack_30)));
    if (SQRT(fVar2) < 1.5) {
      RT_CALLRA(0x25c02c, guest::PlayUiSound(0x302));
      RT_CALLRA(0x25c03c, guest::SetFlaggedSlotBit(0xf,0x17,0));
    }
    RT_CALLRA(0x25c044, guest::vu0UnitMatrix(auStack_70));
    RT_CALLRA(0x25c054, guest::tileSlotOrigin(auStack_40,0x2c4120,3));
    RT_CALLRA(0x25c05c, guest::vifObjectMatrix(auStack_70));
    RT_CALLRA(0x25c06c, guest::vifCallModel((*(uint32_t*)rtguest::G(0x177f5d0)) + 0x10));
  }
  return 0;
}


RT_GUEST_FN(0x0025be98, guest::drawProximityEventProp)

}  // namespace guest
