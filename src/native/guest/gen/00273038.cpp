// guest:0x00273038 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t vu0DropShadowMatrix(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
  rtguest::FrameGuard rt_fg;
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  
  if (param_6 != 0) {
    fVar1 = (*(float*)rtguest::G((uint32_t)param_5));
    fVar4 = (*(float*)rtguest::G((uint32_t)param_5 + (uint32_t)(1) * sizeof(float)));
    fVar3 = (*(float*)rtguest::G((uint32_t)param_5 + (uint32_t)(2) * sizeof(float)));
    (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(3) * sizeof(float))) = param_1;
    (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(7) * sizeof(float))) = param_2;
    (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(0xb) * sizeof(float))) = param_3;
    (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(0xc) * sizeof(float))) = -fVar1;
    (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(0xd) * sizeof(float))) = -fVar4;
    (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(0xe) * sizeof(float))) = -fVar3;
    fVar2 = 1.0 - (param_1 * fVar1 + param_2 * fVar4 + param_3 * fVar3);
    (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(4) * sizeof(float))) = param_2 * fVar1;
    (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(float))) = param_1 * fVar4;
    (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(8) * sizeof(float))) = param_3 * fVar1;
    (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(9) * sizeof(float))) = param_3 * fVar4;
    (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(0xf) * sizeof(float))) = fVar2 - 1.0;
    (*(float*)rtguest::G((uint32_t)param_4)) = param_1 * fVar1 + fVar2;
    (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(5) * sizeof(float))) = param_2 * fVar4 + fVar2;
    (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(10) * sizeof(float))) = param_3 * fVar3 + fVar2;
    (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(2) * sizeof(float))) = param_1 * fVar3;
    (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(6) * sizeof(float))) = param_2 * fVar3;
    return 0;
  }
  fVar3 = (*(float*)rtguest::G((uint32_t)param_5));
  fVar4 = (*(float*)rtguest::G((uint32_t)param_5 + (uint32_t)(1) * sizeof(float)));
  fVar5 = (*(float*)rtguest::G((uint32_t)param_5 + (uint32_t)(2) * sizeof(float)));
  (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(3) * sizeof(float))) = 0.0;
  (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(7) * sizeof(float))) = 0.0;
  (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(0xb) * sizeof(float))) = 0.0;
  fVar2 = param_1 * fVar3 + param_2 * fVar4 + param_3 * fVar5;
  fVar1 = -1.0 / fVar2;
  (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(0xf) * sizeof(float))) = fVar1 * -fVar2;
  (*(float*)rtguest::G((uint32_t)param_4)) = fVar1 * (param_1 * fVar3 - fVar2);
  (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(4) * sizeof(float))) = fVar1 * param_2 * fVar3;
  (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(8) * sizeof(float))) = fVar1 * param_3 * fVar3;
  (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(0xc) * sizeof(float))) = fVar1 * -fVar3;
  (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(float))) = fVar1 * param_1 * fVar4;
  (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(5) * sizeof(float))) = fVar1 * (param_2 * fVar4 - fVar2);
  (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(9) * sizeof(float))) = fVar1 * param_3 * fVar4;
  (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(0xd) * sizeof(float))) = fVar1 * -fVar4;
  (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(2) * sizeof(float))) = fVar1 * param_1 * fVar5;
  (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(6) * sizeof(float))) = fVar1 * param_2 * fVar5;
  (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(10) * sizeof(float))) = fVar1 * (param_3 * fVar5 - fVar2);
  (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(0xe) * sizeof(float))) = fVar1 * -fVar5;
  return 0;
}


RT_GUEST_FN(0x00273038, guest::vu0DropShadowMatrix)

}  // namespace guest
