// guest:0x0025d328 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawFieldProp16(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  long lVar1;
uint32_t auStack_50 = rtguest::allocFrame(48);
uint32_t auStack_20 = rtguest::allocFrame(16);
  
  vifUploadColours(0x2c4210,0x2c4220);
  if (((7 < (*(int8_t*)rtguest::G(0x335738))) || ((*(int8_t*)rtguest::G(0x335738)) < 6)) && (lVar1 = TestFlagBitFc98(0x50), lVar1 == 0)) {
    vifFixedRegs(0x200696c5dd30b476,7,0,0xf);
    vu0UnitMatrix(auStack_50);
    tileSlotOrigin(auStack_20,0x2a2bb0,2);
    vifObjectMatrix(auStack_50);
    drawModelParts((*(int32_t*)rtguest::G(0x177f5b0)),3);
  }
  return 0;
}


RT_GUEST_FN(0x0025d328, guest::drawFieldProp16)

}  // namespace guest
