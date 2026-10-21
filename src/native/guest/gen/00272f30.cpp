// guest:0x00272f30 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t vu0ViewScreenMatrix(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, uint32_t param_7, uint32_t param_8, uint32_t param_9, ...) {
  rtguest::FrameGuard rt_fg(160);
uint32_t auStack_a0 = rtguest::allocFrame(20);
    uint32_t uStack_8c = rtguest::allocFrame(4);
    rtguest::allocFrame(16);  // guest frame gap
    uint32_t fStack_78 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_70 = rtguest::allocFrame(4);
    uint32_t uStack_6c = rtguest::allocFrame(4);
    uint32_t fStack_68 = rtguest::allocFrame(4);
uint32_t puVar1;
  float fVar2;
  float in_stack_00000000;
  
  fVar2 = rtguest::fdiv((in_stack_00000000 * param_8 * (-param_6 + param_7)), (-param_8 + in_stack_00000000));  // fn/00272f30.c div.s @0x272f9c (round-nearest)
  RT_CALLRA(0x272fb0, guest::vu0UnitMatrix(param_9));  // MIPS 0x272fb0: $a0 = $s0 = same buffer as puVar1=param_9 identity stores
  (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(5) * sizeof(uint32_t))) = param_1;
  (*(uint32_t*)rtguest::G((uint32_t)puVar1)) = param_1;
  (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(10) * sizeof(uint32_t))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(0xf) * sizeof(uint32_t))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(0xb) * sizeof(uint32_t))) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(0xe) * sizeof(uint32_t))) = 0x3f800000;
  RT_CALLRA(0x272fd8, guest::vu0UnitMatrix(auStack_a0));
  (*(uint32_t*)rtguest::G((uint32_t)auStack_a0 + (uint32_t)(0) * 4)) = param_2;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_8c)) = param_3;
  (*(float*)rtguest::G((uint32_t)fStack_78)) = fVar2;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_70)) = param_4;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_6c)) = param_5;
  (*(float*)rtguest::G((uint32_t)fStack_68)) = rtguest::fdiv((-param_7 * param_8 + param_6 * in_stack_00000000), (-param_8 + in_stack_00000000));  // div.s @0x272fa8 (round-nearest)
  RT_CALLRA(0x273000, guest::vu0MulMatrix(param_9,auStack_a0,param_9));
  return 0;
}


RT_GUEST_FN(0x00272f30, guest::vu0ViewScreenMatrix)

}  // namespace guest
