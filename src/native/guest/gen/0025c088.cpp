// guest:0x0025c088 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawFieldProp0b(uint32_t q0, ...) {
  long lVar1;
uint32_t auStack_50 = rtguest::allocFrame(48);
uint32_t auStack_20 = rtguest::allocFrame(16);
  
  lVar1 = TestStateBitFc78(0x29);
  if (lVar1 != 0) {
    vifFixedRegs(0x20078e05dd30bc30,7,4,9);
    vu0UnitMatrix(auStack_50);
    tileSlotOrigin(auStack_20,0x2c4130,3);
    vifObjectMatrix(auStack_50);
    vifCallModel((*(int32_t*)rtguest::G(0x177f5c8)) + 0x10);
  }
  lVar1 = TestFlagBitFc98(0x50);
  if (lVar1 != 0) {
    vifFixedRegs(0x20079685dd30bc74,7,0,0xf);
    vu0UnitMatrix(auStack_50);
    tileSlotOrigin(auStack_20,0x2c4140,3);
    vifObjectMatrix(auStack_50);
    vifCallModel((*(uint32_t*)rtguest::G(0x177f5cc)) + 0x10);
  }
  lVar1 = TestStateBitFc78(0x32);
  if (lVar1 != 0) {
    vifFixedRegs(0x20079f05dd30bcb8,7,0,0xf);
    vu0UnitMatrix(auStack_50);
    tileSlotOrigin(auStack_20,0x2c4150,3);
    vifObjectMatrix(auStack_50);
    vifCallModel((*(uint32_t*)rtguest::G(0x177f5d0)) + 0x10);
  }
  return 0;
}


RT_GUEST_FN(0x0025c088, guest::drawFieldProp0b)

}  // namespace guest
