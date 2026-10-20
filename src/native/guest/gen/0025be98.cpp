// guest:0x0025be98 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawProximityEventProp(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(112);
  long lVar1;
  float fVar2;
uint32_t auStack_70 = rtguest::allocFrame(48);
uint32_t auStack_40 = rtguest::allocFrame(16);
uint32_t auStack_30 = rtguest::allocFrame(16);
  
  vifFixedRegs(0x20079f05dd30bcb8,7,0,0xf);
  if ((*(int8_t*)rtguest::G(0x33573a)) == 4) {
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) & 1) != 0) {
      vu0UnitMatrix(auStack_70);
      tileSlotOrigin(auStack_40,0x2a2bc0,3);
      vifObjectMatrix(auStack_70);
      vifCallModel((*(int32_t*)rtguest::G(0x177f5c8)) + 0x10);
    }
  }
  else {
    if ((*(int8_t*)rtguest::G(0x33573a)) < 5) {
      if ((*(int8_t*)rtguest::G(0x33573a)) == 0) goto LAB_0025bf98;
    }
    else if ((*(int8_t*)rtguest::G(0x33573a)) == 5) goto LAB_0025bf98;
    vu0UnitMatrix(auStack_70);
    tileSlotOrigin(auStack_40,0x2a2bc0,3);
    vifObjectMatrix(auStack_70);
    vifCallModel((*(int32_t*)rtguest::G(0x177f5c8)) + 0x10);
  }
LAB_0025bf98:
  lVar1 = GetFlaggedSlotCount(0xf,0x17,0);
  if ((lVar1 == 0) && (lVar1 = TestStateBitFc78(5), lVar1 == 0)) {
    vu0SubVector(auStack_30,0x2c4120,0x177ace0);
    fVar2 = rtguest::fbits(vecDotProductVu0(auStack_30,auStack_30));
    if (SQRT(fVar2) < 1.5) {
      PlayUiSound(0x302);
      SetFlaggedSlotBit(0xf,0x17,0);
    }
    vu0UnitMatrix(auStack_70);
    tileSlotOrigin(auStack_40,0x2c4120,3);
    vifObjectMatrix(auStack_70);
    vifCallModel((*(uint32_t*)rtguest::G(0x177f5d0)) + 0x10);
  }
  return 0;
}


RT_GUEST_FN(0x0025be98, guest::drawProximityEventProp)

}  // namespace guest
