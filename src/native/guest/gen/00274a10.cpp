// guest:0x00274a10 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sinf(uint32_t param_1, ...) {
    uint32_t uStack_20 = rtguest::allocFrame(4);
    uint32_t uStack_2c = rtguest::allocFrame(4);
    uint32_t uStack_30 = rtguest::allocFrame(4);
  ulong uVar1;
  float fVar2;
  
  (*(uint32_t*)rtguest::G((uint32_t)uStack_20)) = param_1;
  if ((param_1 & 0x7fffffff) < 0x3f490fd9) {
    fVar2 = (float)__kernel_sinf(param_1,0,0);
  }
  uVar1 = __ieee754_rem_pio2f(uStack_30);
  uVar1 = uVar1 & 3;
  if (uVar1 == 1) {
    fVar2 = (float)__kernel_cosf((*(uint32_t*)rtguest::G((uint32_t)uStack_30)),(*(uint32_t*)rtguest::G((uint32_t)uStack_2c)));
  }
  if (uVar1 < 2) {
    if (uVar1 == 0) {
      fVar2 = (float)__kernel_sinf((*(uint32_t*)rtguest::G((uint32_t)uStack_30)),(*(uint32_t*)rtguest::G((uint32_t)uStack_2c)),1);
    }
  }
  else if (uVar1 == 2) {
    fVar2 = (float)__kernel_sinf((*(uint32_t*)rtguest::G((uint32_t)uStack_30)),(*(uint32_t*)rtguest::G((uint32_t)uStack_2c)),1);
    goto LAB_00274aec;
  }
  fVar2 = (float)__kernel_cosf((*(uint32_t*)rtguest::G((uint32_t)uStack_30)),(*(uint32_t*)rtguest::G((uint32_t)uStack_2c)));
LAB_00274aec:
  return -fVar2;
}


RT_GUEST_FN(0x00274a10, guest::sinf)

}  // namespace guest
