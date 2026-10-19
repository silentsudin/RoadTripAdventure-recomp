// guest:0x00249458 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CalculateSpeedometerValue(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  int iVar1;
  float fVar2;
  
  fVar2 = (float)GetSpeedFromF15Vector(param_1 + 0xf0);
  iVar1 = (int)(fVar2 / (*(float*)rtguest::G(0x332f44)));
  if (((param_2 != 0) || (param_3 != 0)) && (0xba < iVar1)) {
    iVar1 = 0xba;
  }
  if (param_2 != 0) {
    iVar1 = (iVar1 * (int)param_2 + 0xb9) / 0xba;
  }
}


RT_GUEST_FN(0x00249458, guest::CalculateSpeedometerValue)

}  // namespace guest
