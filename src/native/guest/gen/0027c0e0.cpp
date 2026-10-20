// guest:0x0027c0e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __make_dp(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint64_t param_4, ...) {
  rtguest::FrameGuard rt_fg(48);
    // __pack_d's in-struct is contiguous: class+0, sign+4, exp+8, pad+0xc, u64 frac+0x10
    uint32_t uStack_30 = rtguest::allocFrame(0x18);
  
  (*(uint32_t*)rtguest::G((uint32_t)uStack_30 + (uint32_t)(0) * 4)) = param_1;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_30 + (uint32_t)(1) * 4)) = param_2;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_30 + (uint32_t)(2) * 4)) = param_3;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_30 + (uint32_t)(4) * 4)) = param_4;
  __pack_d(uStack_30);
  return 0;
}


RT_GUEST_FN(0x0027c0e0, guest::__make_dp)

}  // namespace guest
