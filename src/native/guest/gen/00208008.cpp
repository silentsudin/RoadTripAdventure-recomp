// guest:0x00208008 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t BuildYawTransform(uint32_t param_1, uint32_t param_2, ...) {
uint32_t pfVar1;
  
  pfVar1 = (uint32_t)param_1;
  vu0UnitMatrix();
  (*(float*)rtguest::G((uint32_t)pfVar1)) = (*(float*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * 4));
  (*(float*)rtguest::G((uint32_t)pfVar1 + (uint32_t)(1) * sizeof(float))) = -(*(float*)rtguest::G((uint32_t)(uint32_t)param_2));
  vecNormalizeVu0(param_1,param_1);
  vu0CopyVector(pfVar1 + 4 * sizeof(float),param_2);
  vecCrossProductVu0(pfVar1 + 8 * sizeof(float),param_1,pfVar1 + 4 * sizeof(float));
  return 0;
}


RT_GUEST_FN(0x00208008, guest::BuildYawTransform)

}  // namespace guest
