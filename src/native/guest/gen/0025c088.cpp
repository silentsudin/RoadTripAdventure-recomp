// guest:0x0025c088 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawFieldProp0b(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x40) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x44) = 0x00000000u;
  }
  long lVar1;
uint32_t auStack_50 = rtguest::allocFrame(48);
uint32_t auStack_20 = rtguest::allocFrame(16);
  
  lVar1 = RT_CALLRA(0x25c090, guest::TestStateBitFc78(0x29));
  if (lVar1 != 0) {
    RT_CALLRA(0x25c0c0, guest::vifFixedRegs(0x20078e05dd30bc30,7,4,9));
    RT_CALLRA(0x25c0c8, guest::vu0UnitMatrix(auStack_50));
    RT_CALLRA(0x25c0dc, guest::tileSlotOrigin(auStack_20,0x2c4130,3));
    RT_CALLRA(0x25c0e4, guest::vifObjectMatrix(auStack_50));
    RT_CALLRA(0x25c0f4, guest::vifCallModel((*(int32_t*)rtguest::G(0x177f5c8)) + 0x10));
  }
  lVar1 = RT_CALLRA(0x25c0fc, guest::TestFlagBitFc98(0x50));
  if (lVar1 != 0) {
    RT_CALLRA(0x25c128, guest::vifFixedRegs(0x20079685dd30bc74,7,0,0xf));
    RT_CALLRA(0x25c130, guest::vu0UnitMatrix(auStack_50));
    RT_CALLRA(0x25c144, guest::tileSlotOrigin(auStack_20,0x2c4140,3));
    RT_CALLRA(0x25c14c, guest::vifObjectMatrix(auStack_50));
    RT_CALLRA(0x25c15c, guest::vifCallModel((*(uint32_t*)rtguest::G(0x177f5cc)) + 0x10));
  }
  lVar1 = RT_CALLRA(0x25c164, guest::TestStateBitFc78(0x32));
  if (lVar1 != 0) {
    RT_CALLRA(0x25c190, guest::vifFixedRegs(0x20079f05dd30bcb8,7,0,0xf));
    RT_CALLRA(0x25c198, guest::vu0UnitMatrix(auStack_50));
    RT_CALLRA(0x25c1ac, guest::tileSlotOrigin(auStack_20,0x2c4150,3));
    RT_CALLRA(0x25c1b4, guest::vifObjectMatrix(auStack_50));
    RT_CALLRA(0x25c1c4, guest::vifCallModel((*(uint32_t*)rtguest::G(0x177f5d0)) + 0x10));
  }
  return 0;
}


RT_GUEST_FN(0x0025c088, guest::drawFieldProp0b)

}  // namespace guest
