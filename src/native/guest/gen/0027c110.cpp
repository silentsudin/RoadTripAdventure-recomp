// guest:0x0027c110 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t dptofp(uint64_t param_1, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = (uint32_t)(int32_t)param_1;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = (uint32_t)((int64_t)(int32_t)param_1 >> 32);
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
  }
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
  RT_CALLRA(0x27c120, guest::__unpack_d(auStack_20,uStack_40));
  uVar1 = (long)((*(uint64_t*)rtguest::G((uint32_t)uStack_30)) << 2) >> 0x20;
  uVar2 = uVar1 | 1;
  if (((*(uint64_t*)rtguest::G((uint32_t)uStack_30)) & 0x3fffffff) == 0) {
    uVar2 = uVar1;
  }
  RT_CALLRA(0x27c150, guest::__make_fp((*(uint32_t*)rtguest::G((uint32_t)uStack_40)),(*(uint32_t*)rtguest::G((uint32_t)uStack_3c)),(*(uint32_t*)rtguest::G((uint32_t)uStack_38)),uVar2));
  return 0;
}


RT_GUEST_FN(0x0027c110, guest::dptofp)

}  // namespace guest
