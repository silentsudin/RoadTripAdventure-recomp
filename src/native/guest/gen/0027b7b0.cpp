// guest:0x0027b7b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t fptodp(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
    // __unpack_f's out-words are contiguous (class+0 sign+4 exp+8 frac+0xc)
    uint32_t uStack_30 = rtguest::allocFrame(16);
uint32_t auStack_20 = rtguest::allocFrame(16);
  
  (*(uint32_t*)rtguest::G((uint32_t)auStack_20 + (uint32_t)(0) * 4)) = param_1;
  RT_CALLRA(0x27b7c0, guest::__unpack_f(auStack_20,uStack_30));
  RT_CALLRA(0x27b7d8, guest::__make_dp((*(uint32_t*)rtguest::G((uint32_t)uStack_30)),(*(uint32_t*)rtguest::G((uint32_t)uStack_30 + (uint32_t)(1) * 4)),(*(uint32_t*)rtguest::G((uint32_t)uStack_30 + (uint32_t)(2) * 4)),(uint64_t)(*(uint32_t*)rtguest::G((uint32_t)uStack_30 + (uint32_t)(3) * 4)) << 0x1e));
  return 0;
}


RT_GUEST_FN(0x0027b7b0, guest::fptodp)

}  // namespace guest
