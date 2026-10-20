// guest:0x00274a10 translated from Ghidra pseudocode and ELF disassembly
// f-register convention: params/returns carry float bits (register transfers
// never value-convert); float math goes through rtguest::fbits/f2bits.
// 0x2752cc: sign test is blez on x's raw bits (bgtz form) -> (int32_t)param_1.
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t sinf(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
    uint32_t uStack_30 = rtguest::allocFrame(4);
    uint32_t uStack_2c = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_20 = rtguest::allocFrame(4);
  uint32_t uVar1;
  float fVar2;

  (*(uint32_t*)rtguest::G((uint32_t)uStack_20)) = param_1;
  if ((param_1 & 0x7fffffffu) < 0x3f490fd9u) {
    return rtguest::f2bits(rtguest::fbits(RT_CALLRA(0x274a48, guest::__kernel_sinf(param_1,0,0))));
  }
  // pseudocode omitted the f12 input arg; the call passes x's bits then the
  // hi-slot pointer (lo lands at ptr+4, matching uStack_30/uStack_2c order).
  uVar1 = RT_CALLRA(0x274a58, guest::__ieee754_rem_pio2f(param_1,uStack_30));
  uVar1 = uVar1 & 3;
  if (uVar1 == 1) {
    fVar2 = rtguest::fbits(RT_CALLRA(0x274ab4, guest::__kernel_cosf((*(uint32_t*)rtguest::G((uint32_t)uStack_30)),(*(uint32_t*)rtguest::G((uint32_t)uStack_2c)))));
    return rtguest::f2bits(fVar2);  // n==1: +cos, no flip (fn/00274a10.c:25; newlib sf_sin.c)
  }
  if (uVar1 < 2) {
    if (uVar1 == 0) {
      fVar2 = rtguest::fbits(RT_CALLRA(0x274a9c, guest::__kernel_sinf((*(uint32_t*)rtguest::G((uint32_t)uStack_30)),(*(uint32_t*)rtguest::G((uint32_t)uStack_2c)),1)));
      return rtguest::f2bits(fVar2);
    }
  }
  else if (uVar1 == 2) {
    fVar2 = rtguest::fbits(RT_CALLRA(0x274acc, guest::__kernel_sinf((*(uint32_t*)rtguest::G((uint32_t)uStack_30)),(*(uint32_t*)rtguest::G((uint32_t)uStack_2c)),1)));
    return rtguest::f2bits(-fVar2);
  }
  fVar2 = rtguest::fbits(RT_CALLRA(0x274ae0, guest::__kernel_cosf((*(uint32_t*)rtguest::G((uint32_t)uStack_30)),(*(uint32_t*)rtguest::G((uint32_t)uStack_2c)))));
  return rtguest::f2bits(-fVar2);  // n==3: -cos (fn/00274a10.c:41 LAB_00274aec neg.s)
}


RT_GUEST_FN(0x00274a10, guest::sinf)

}  // namespace guest
