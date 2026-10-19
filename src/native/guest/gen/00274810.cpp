// guest:0x00274810 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t cos(uint32_t __x, ...) {
    uint32_t uStack_20 = rtguest::allocFrame(8);
    uint32_t uStack_18 = rtguest::allocFrame(8);
  double dVar1;
  undefined8 uVar2;
  ulong uVar3;
  
  uVar3 = (long)__x >> 0x20 & 0x7fffffff;
  if (uVar3 < 0x3fe921fc) {
    dVar1 = (double)__kernel_cos(__x,0);
    return dVar1;
  }
  if (0x7fefffff < uVar3) {
    dVar1 = (double)dpsub();
    return dVar1;
  }
  uVar3 = __ieee754_rem_pio2(__x,uStack_20);
  uVar3 = uVar3 & 3;
  if (uVar3 == 1) {
    uVar2 = __kernel_sin((*(uint64_t*)rtguest::G((uint32_t)uStack_20)),(*(uint64_t*)rtguest::G((uint32_t)uStack_18)),1);
LAB_00274900:
    dVar1 = (double)dpsub(0,uVar2);
  }
  else {
    if (uVar3 < 2) {
      if (uVar3 == 0) {
        dVar1 = (double)__kernel_cos((*(uint64_t*)rtguest::G((uint32_t)uStack_20)),(*(uint64_t*)rtguest::G((uint32_t)uStack_18)));
        return dVar1;
      }
    }
    else if (uVar3 == 2) {
      uVar2 = __kernel_cos((*(uint64_t*)rtguest::G((uint32_t)uStack_20)),(*(uint64_t*)rtguest::G((uint32_t)uStack_18)));
      goto LAB_00274900;
    }
    dVar1 = (double)__kernel_sin((*(uint64_t*)rtguest::G((uint32_t)uStack_20)),(*(uint64_t*)rtguest::G((uint32_t)uStack_18)),1);
  }
  return dVar1;
}


RT_GUEST_FN(0x00274810, guest::cos)

}  // namespace guest
