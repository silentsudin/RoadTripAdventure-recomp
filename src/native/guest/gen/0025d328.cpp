// guest:0x0025d328 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawFieldProp16(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x40) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x44) = 0x00000000u;
  }
  long lVar1;
uint32_t auStack_50 = rtguest::allocFrame(48);
uint32_t auStack_20 = rtguest::allocFrame(16);
  
  RT_CALLRA(0x25d33c, guest::vifUploadColours(0x2c4210,0x2c4220));
  if (((7 < (*(int8_t*)rtguest::G(0x335738))) || ((*(int8_t*)rtguest::G(0x335738)) < 6)) && (lVar1 = TestFlagBitFc98(0x50), lVar1 == 0)) {
    RT_CALLRA(0x25d390, guest::vifFixedRegs(0x200696c5dd30b476,7,0,0xf));
    RT_CALLRA(0x25d398, guest::vu0UnitMatrix(auStack_50));
    RT_CALLRA(0x25d3ac, guest::tileSlotOrigin(auStack_20,0x2a2bb0,2));
    RT_CALLRA(0x25d3b4, guest::vifObjectMatrix(auStack_50));
    RT_CALLRA(0x25d3c4, guest::drawModelParts((*(int32_t*)rtguest::G(0x177f5b0)),3));
  }
  return 0;
}


RT_GUEST_FN(0x0025d328, guest::drawFieldProp16)

}  // namespace guest
