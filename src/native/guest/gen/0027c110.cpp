// guest:0x0027c110 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t dptofp(uint64_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
    uint32_t uStack_40 = rtguest::allocFrame(4);
    uint32_t uStack_3c = rtguest::allocFrame(4);
    uint32_t uStack_38 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_30 = rtguest::allocFrame(8);
    rtguest::allocFrame(8);  // guest frame gap
uint32_t auStack_20 = rtguest::allocFrame(16);
  ulong uVar1;
  ulong uVar2;
  
  (*(uint64_t*)rtguest::G((uint32_t)auStack_20 + (uint32_t)(0) * 8)) = param_1;
  __unpack_d(auStack_20,uStack_40);
  uVar1 = (long)((*(uint64_t*)rtguest::G((uint32_t)uStack_30)) << 2) >> 0x20;
  uVar2 = uVar1 | 1;
  if (((*(uint64_t*)rtguest::G((uint32_t)uStack_30)) & 0x3fffffff) == 0) {
    uVar2 = uVar1;
  }
  __make_fp((*(uint32_t*)rtguest::G((uint32_t)uStack_40)),(*(uint32_t*)rtguest::G((uint32_t)uStack_3c)),(*(uint32_t*)rtguest::G((uint32_t)uStack_38)),uVar2);
  return 0;
}


RT_GUEST_FN(0x0027c110, guest::dptofp)

}  // namespace guest
