// guest:0x002494e8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CalculateSpeedometerDigits(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  float fVar1;
  
  fVar1 = rtguest::fbits(GetSpeedFromF15Vector(param_1 + 0xf0));
  IntToPackedBCD((int)(fVar1 / (*(float*)rtguest::G(0x332f48))));
  return 0;
}


RT_GUEST_FN(0x002494e8, guest::CalculateSpeedometerDigits)

}  // namespace guest
