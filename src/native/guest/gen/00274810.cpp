// guest:0x00274810 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint64_t cos(uint64_t __x, ...) {  // double travels as GPR bits (pseudocode 0x274810: double cos(double); dsra32 at 0x27481c reads the high word)
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
    uint32_t uStack_20 = rtguest::allocFrame(8);
    uint32_t uStack_18 = rtguest::allocFrame(8);
  uint64_t dVar1;  // pseudocode 0x274810: double is a soft-float GPR bit pattern (no cvt); numeric (double) cast was UB
  undefined8 uVar2;
  ulong uVar3;
  
  uVar3 = (long)__x >> 0x20 & 0x7fffffff;
  if (uVar3 < 0x3fe921fc) {
    dVar1 = RT_CALLRA(0x274844, guest::__kernel_cos(__x,0));
    return dVar1;
  }
  if (0x7fefffff < uVar3) {
    dVar1 = RT_CALLRA(0x27486c, guest::dpsub());
    return dVar1;
  }
  uVar3 = RT_CALLRA(0x274880, guest::__ieee754_rem_pio2(__x,uStack_20));
  uVar3 = uVar3 & 3;
  if (uVar3 == 1) {
    uVar2 = RT_CALLRA(0x2748e0, guest::__kernel_sin((*(uint64_t*)rtguest::G((uint32_t)uStack_20)),(*(uint64_t*)rtguest::G((uint32_t)uStack_18)),1));
LAB_00274900:
    dVar1 = RT_CALLRA(0x274908, guest::dpsub(0,uVar2));
  }
  else {
    if (uVar3 < 2) {
      if (uVar3 == 0) {
        dVar1 = RT_CALLRA(0x2748c8, guest::__kernel_cos((*(uint64_t*)rtguest::G((uint32_t)uStack_20)),(*(uint64_t*)rtguest::G((uint32_t)uStack_18))));
        return dVar1;
      }
    }
    else if (uVar3 == 2) {
      uVar2 = RT_CALLRA(0x2748f8, guest::__kernel_cos((*(uint64_t*)rtguest::G((uint32_t)uStack_20)),(*(uint64_t*)rtguest::G((uint32_t)uStack_18))));
      goto LAB_00274900;
    }
    dVar1 = RT_CALLRA(0x274920, guest::__kernel_sin((*(uint64_t*)rtguest::G((uint32_t)uStack_20)),(*(uint64_t*)rtguest::G((uint32_t)uStack_18)),1));
  }
  return dVar1;
}


RT_GUEST_FN(0x00274810, guest::cos)

}  // namespace guest
