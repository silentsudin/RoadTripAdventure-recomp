// guest:0x00272ce0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t vu0RotMatrix(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  vu0RotMatrixZ(*(uint32_t*)rtguest::G((uint32_t)(param_3 + 8)));
  vu0RotMatrixY(*(uint32_t*)rtguest::G((uint32_t)(param_3 + 4)), param_1, param_1);
  vu0RotMatrixX(*(uint32_t*)rtguest::G((uint32_t)param_3), param_1, param_1);
  return 0;
}


RT_GUEST_FN(0x00272ce0, guest::vu0RotMatrix)

}  // namespace guest
