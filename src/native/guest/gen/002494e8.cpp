// guest:0x002494e8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CalculateSpeedometerDigits(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
  float fVar1;
  
  fVar1 = rtguest::fbits(RT_CALLRA(0x2494f0, guest::GetSpeedFromF15Vector(param_1 + 0xf0)));
  IntToPackedBCD((int)(rtguest::fdiv(fVar1, (*(float*)rtguest::G(0x332f48)))));
  return 0;
}


RT_GUEST_FN(0x002494e8, guest::CalculateSpeedometerDigits)

}  // namespace guest
