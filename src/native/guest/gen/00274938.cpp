// guest:0x00274938 translated from Ghidra pseudocode and ELF disassembly
// f-register convention: params/returns carry float bits; rtguest::fbits/f2bits
// bridge to float math (see 00274a10 header note).
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t cosf(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
    uint32_t uStack_30 = rtguest::allocFrame(4);
    uint32_t uStack_2c = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_20 = rtguest::allocFrame(4);
  uint32_t uVar1;
  float fVar2;

  (*(uint32_t*)rtguest::G((uint32_t)uStack_20)) = param_1;
  if ((param_1 & 0x7fffffffu) < 0x3f490fd9u) {
    return rtguest::f2bits(rtguest::fbits(__kernel_cosf(param_1,0)));
  }
  // f12 input arg elided in pseudocode: pass x's bits then the hi pointer.
  uVar1 = __ieee754_rem_pio2f(param_1,uStack_30);
  uVar1 = uVar1 & 3;
  if (uVar1 == 1) {
    fVar2 = rtguest::fbits(__kernel_sinf((*(uint32_t*)rtguest::G((uint32_t)uStack_30)),(*(uint32_t*)rtguest::G((uint32_t)uStack_2c)),1));
    fVar2 = -fVar2;
  }
  else {
    if (uVar1 < 2) {
      if (uVar1 == 0) {
        fVar2 = rtguest::fbits(__kernel_cosf((*(uint32_t*)rtguest::G((uint32_t)uStack_30)),(*(uint32_t*)rtguest::G((uint32_t)uStack_2c))));
        return rtguest::f2bits(fVar2);
      }
    }
    else if (uVar1 == 2) {
      fVar2 = rtguest::fbits(__kernel_cosf((*(uint32_t*)rtguest::G((uint32_t)uStack_30)),(*(uint32_t*)rtguest::G((uint32_t)uStack_2c))));
      return rtguest::f2bits(-fVar2);
    }
    fVar2 = rtguest::fbits(__kernel_sinf((*(uint32_t*)rtguest::G((uint32_t)uStack_30)),(*(uint32_t*)rtguest::G((uint32_t)uStack_2c)),1));
  }
  return rtguest::f2bits(fVar2);
}


RT_GUEST_FN(0x00274938, guest::cosf)

}  // namespace guest
