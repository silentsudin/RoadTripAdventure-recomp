// guest:0x0021c728 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ToRelativeVec4(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(0);
  float fVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  
  fVar1 = (*(float*)rtguest::G(0x332c1c));
  uVar2 = (*(int32_t*)rtguest::G((uint32_t)param_2)) - param_3;
  uVar3 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t))) - param_4;
  if ((uVar2 & 0x8000000) == 0) {
    uVar2 = uVar2 & 0x7ffffff;
  }
  else {
    uVar2 = uVar2 | 0xf8000000;
  }
  if ((uVar3 & 0x8000000) == 0) {
    uVar3 = uVar3 & 0x7ffffff;
  }
  else {
    uVar3 = uVar3 | 0xf8000000;
  }
  iVar4 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t)));
  (*(float*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(float))) = 1.0;
  (*(float*)rtguest::G((uint32_t)param_1)) = rtguest::fdiv((float)(int)uVar2, fVar1);
  (*(float*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(float))) = rtguest::fdiv((float)(int)uVar3, fVar1);
  (*(float*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(float))) = rtguest::fdiv((float)iVar4, fVar1);
  return 0;
}


RT_GUEST_FN(0x0021c728, guest::ToRelativeVec4)

}  // namespace guest
