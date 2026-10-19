// guest:0x00240be0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t IsPointInsideQuad(uint32_t param_1, uint32_t param_2, ...) {
  undefined4 uVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  
  fVar2 = (*(float*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(float)));
  fVar3 = (*(float*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(float)));
  fVar6 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x98)));
  fVar7 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x90)));
  fVar8 = (*(float*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(float)));
  fVar9 = (*(float*)rtguest::G((uint32_t)param_1));
  uVar1 = 0;
  if (0.0 < (fVar3 - fVar8) * (fVar7 - fVar2) - (fVar2 - fVar9) * (fVar6 - fVar3)) {
    fVar4 = (*(float*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(float)));
    fVar5 = (*(float*)rtguest::G((uint32_t)param_1 + (uint32_t)(5) * sizeof(float)));
    if (0.0 < (fVar5 - fVar3) * (fVar7 - fVar4) - (fVar4 - fVar2) * (fVar6 - fVar5)) {
      fVar3 = (*(float*)rtguest::G((uint32_t)param_1 + (uint32_t)(6) * sizeof(float)));
      fVar2 = (*(float*)rtguest::G((uint32_t)param_1 + (uint32_t)(7) * sizeof(float)));
      if ((0.0 < (fVar2 - fVar5) * (fVar7 - fVar3) - (fVar3 - fVar4) * (fVar6 - fVar2)) &&
         (0.0 < (fVar8 - fVar2) * (fVar7 - fVar9) - (fVar9 - fVar3) * (fVar6 - fVar8))) {
        uVar1 = 1;
      }
    }
  }
}


RT_GUEST_FN(0x00240be0, guest::IsPointInsideQuad)

}  // namespace guest
