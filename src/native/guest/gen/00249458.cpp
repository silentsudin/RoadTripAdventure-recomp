// guest:0x00249458 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CalculateSpeedometerValue(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_2);  // walk: = param_2
  }
  int iVar1;
  float fVar2;
  
  fVar2 = rtguest::fbits(RT_CALLRA(0x249470, guest::GetSpeedFromF15Vector(param_1 + 0xf0)));
  iVar1 = (int)(rtguest::fdiv(fVar2, (*(float*)rtguest::G(0x332f44))));
  if (((param_2 != 0) || (param_3 != 0)) && (0xba < iVar1)) {
    iVar1 = 0xba;
  }
  if (param_2 != 0) {
    iVar1 = (iVar1 * (int)param_2 + 0xb9) / 0xba;
  }
  return iVar1;
}


RT_GUEST_FN(0x00249458, guest::CalculateSpeedometerValue)

}  // namespace guest
