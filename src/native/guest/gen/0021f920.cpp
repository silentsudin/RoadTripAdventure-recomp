// guest:0x0021f920 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t mapViewMatrix(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg;
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
uint32_t auStack_90 = rtguest::allocFrame(64);
  
  fVar2 = (*(uint32_t*)rtguest::G(0x332cac));
  fVar4 = (*(float*)rtguest::G(0x332ca8));
  if ((*(uint32_t*)rtguest::G(0x333288)) == 0) {
    fVar4 = 1.0;
  }
  vu0UnitMatrix(param_4);  // MIPS 0x21f978: $a0 = entry $a0 = matrix1 buffer param_4
  fVar3 = 0.0;
  (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(8) * sizeof(float))) = param_1;
  (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(9) * sizeof(float))) = param_2;
  (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(10) * sizeof(float))) = fVar1;
  (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(5) * sizeof(float))) = param_3 * fVar2;
  (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(0xe) * sizeof(float))) = 0.0;
  (*(float*)rtguest::G((uint32_t)param_4)) = param_3 * fVar4;
  (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(0xb) * sizeof(float))) = 1.0;
  (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(0xf) * sizeof(float))) = 0.0;
  vu0UnitMatrix(param_4 + 0x40);  // MIPS 0x21f934 addiu $s1,$s0,0x40; call 0x21f9bc — pseudocode float* 0x10 = 64B
  fVar2 = (*(uint32_t*)rtguest::G(0x332cb8));
  (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(0x1a) * sizeof(float))) = (*(uint32_t*)rtguest::G(0x332cb4));
  (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(0x1e) * sizeof(float))) = fVar2;
  (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(0x1b) * sizeof(float))) = 1.0;
  (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(0x15) * sizeof(float))) = param_3 * 0.001953125;
  (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(0x1f) * sizeof(float))) = fVar3;
  (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(0x10) * sizeof(float))) = param_3 * 0.001953125;
  vu0UnitMatrix(auStack_90);
  fVar2 = (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(10) * sizeof(float)));
  (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(10) * sizeof(float))) = fVar3;
  (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(0xe) * sizeof(float))) = fVar2;
  return 0;
}


RT_GUEST_FN(0x0021f920, guest::mapViewMatrix)

}  // namespace guest
