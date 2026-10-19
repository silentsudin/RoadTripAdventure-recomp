// guest:0x0025c818 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawFieldProp06Pickup(uint32_t param_1, uint32_t param_2, ...) {
  long lVar1;
  float fVar2;
uint32_t auStack_80 = rtguest::allocFrame(48);
uint32_t auStack_50 = rtguest::allocFrame(16);
uint32_t auStack_40 = rtguest::allocFrame(16);
  
  vifFixedRegs(0x200696c5dd30b476,7,0,0xf);
  lVar1 = GetFlaggedSlotCount(0xf,0x22,0);
  if ((lVar1 == 0) && (lVar1 = TestStateBitFc78(9), lVar1 == 0)) {
    vu0SubVector(auStack_40,0x2c41d0,0x177ace0);
    fVar2 = (float)vecDotProductVu0(auStack_40,auStack_40);
    if (SQRT(fVar2) < 1.5) {
      PlayUiSound(0x302);
      SetFlaggedSlotBit(0xf,0x22,0);
    }
    vu0UnitMatrix(auStack_80);
    tileSlotOrigin(auStack_50,0x2c41d0,2);
    vifObjectMatrix(auStack_80);
    vifCallModel((*(int32_t*)rtguest::G(0x177f5b0)) + 0x10);
  }
  if (0x2327f < (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x14))) - 36000U) {
    vu0UnitMatrix(auStack_80);
    vu0RotMatrixY(((float)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) % 0x168 + -0xb4) * (*(uint32_t*)rtguest::G(0x3330d0))) / 180.0,auStack_80
                  ,auStack_80);
    tileSlotOrigin(auStack_50,0x2c41e0,2);
    vifObjectMatrix(auStack_80);
    vifCallModel((*(uint32_t*)rtguest::G(0x177f5b4)) + 0x10);
  }
  return 0;
}


RT_GUEST_FN(0x0025c818, guest::drawFieldProp06Pickup)

}  // namespace guest
