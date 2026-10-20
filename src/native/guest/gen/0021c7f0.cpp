// guest:0x0021c7f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t FixedPosToFloatVec4(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  uint uVar1;
  int iVar2;
  float fVar3;
  float fVar4;
  
  fVar3 = (*(float*)rtguest::G(0x332c20));
  uVar1 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t)));
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_2));
  fVar4 = (float)(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t))) / (*(float*)rtguest::G(0x332c20));
  (*(float*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(float))) = 1.0;
  (*(float*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(float))) = fVar4;
  (*(float*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(float))) = (float)(uVar1 & 0x1ffffff) / fVar3;
  (*(float*)rtguest::G((uint32_t)param_1)) = (float)(iVar2 + ((int)(uVar1 & 0x2000000) >> 1) & 0x1ffffff) / fVar3;
  return 0;
}


RT_GUEST_FN(0x0021c7f0, guest::FixedPosToFloatVec4)

}  // namespace guest
