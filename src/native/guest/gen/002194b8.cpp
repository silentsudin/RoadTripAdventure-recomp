// guest:0x002194b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SteerHeadingVector(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(176);
uint32_t pfVar1;
  float fVar2;
uint32_t auStack_b0 = rtguest::allocFrame(64);
uint32_t auStack_70 = rtguest::allocFrame(64);
  
  pfVar1 = (uint32_t)param_2;
  if ((*(float*)rtguest::G((uint32_t)pfVar1 + (uint32_t)(1) * sizeof(float))) < (*(float*)rtguest::G(0x332b84))) {
    (*(float*)rtguest::G((uint32_t)pfVar1 + (uint32_t)(1) * sizeof(float))) = (*(float*)rtguest::G((uint32_t)pfVar1 + (uint32_t)(1) * sizeof(float))) + (*(uint32_t*)rtguest::G(0x332b88));
  }
  vu0UnitMatrix(auStack_70);
  vu0RotMatrixY(rtguest::f2bits(-rtguest::fbits(param_1)),auStack_b0,auStack_70);
  if ((param_3 & 0x40) == 0) {
    if ((param_3 & 0x80) == 0) {
      (*(float*)rtguest::G((uint32_t)pfVar1)) = (*(float*)rtguest::G((uint32_t)pfVar1)) / 1.0625;
      (*(float*)rtguest::G((uint32_t)pfVar1 + (uint32_t)(2) * sizeof(float))) = (*(float*)rtguest::G((uint32_t)pfVar1 + (uint32_t)(2) * sizeof(float))) / 1.0625;
      goto LAB_0021963c;
    }
    vu0ApplyMatrix(param_2,auStack_b0,param_2);
    (*(float*)rtguest::G((uint32_t)pfVar1 + (uint32_t)(2) * sizeof(float))) = (*(float*)rtguest::G((uint32_t)pfVar1 + (uint32_t)(2) * sizeof(float))) / 1.0625;
    fVar2 = ((*(float*)rtguest::G((uint32_t)pfVar1)) - 0.125) / 1.0625 + 0.125;
  }
  else {
    vu0ApplyMatrix(param_2,auStack_b0,param_2);
    (*(float*)rtguest::G((uint32_t)pfVar1 + (uint32_t)(2) * sizeof(float))) = (*(float*)rtguest::G((uint32_t)pfVar1 + (uint32_t)(2) * sizeof(float))) / 1.0625;
    fVar2 = ((*(float*)rtguest::G((uint32_t)pfVar1)) + 0.125) / 1.0625 - 0.125;
  }
  (*(float*)rtguest::G((uint32_t)pfVar1)) = fVar2;
  vu0RotMatrixY(param_1,auStack_b0,auStack_70);
  vu0ApplyMatrix(param_2,auStack_b0,param_2);
LAB_0021963c:
  vecNormalizeVu0(param_2,param_2);
  return 0;
}


RT_GUEST_FN(0x002194b8, guest::SteerHeadingVector)

}  // namespace guest
