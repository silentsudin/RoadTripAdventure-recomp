// guest:0x0025d2c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t initFieldProp17(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
  (*(uint64_t*)rtguest::G(0x177ea48)) = 0x20079f05e1413cb8;
  vu0UnitMatrix(0x177ebd0);
  tileSlotOrigin(0x177ec00,0x2a2bd0,3);
  return 0;
}


RT_GUEST_FN(0x0025d2c8, guest::initFieldProp17)

}  // namespace guest
