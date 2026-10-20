// guest:0x00207f38 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t WrapCoordinate(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  float fVar1;
  
  fVar1 = (*(float*)rtguest::G((uint32_t)param_1));
  if ((*(float*)rtguest::G(0x332b14)) < fVar1) {
    (*(float*)rtguest::G((uint32_t)param_1)) = fVar1 - (*(float*)rtguest::G(0x332b18));
    return 0;
  }
  if (fVar1 < (*(float*)rtguest::G(0x332b1c))) {
    (*(float*)rtguest::G((uint32_t)param_1)) = fVar1 + (*(float*)rtguest::G(0x332b20));
  }
  return 0;
}


RT_GUEST_FN(0x00207f38, guest::WrapCoordinate)

}  // namespace guest
