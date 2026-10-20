// guest:0x0027d738 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __make_fp(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = (uint32_t)(int32_t)param_1;
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = (uint32_t)(int32_t)param_2;
    *(uint32_t*)rtguest::G(rt_sv+0x8) = (uint32_t)(int32_t)param_3;
    *(uint32_t*)rtguest::G(rt_sv+0xc) = (uint32_t)(int32_t)param_4;
  }
    // __pack_f's in-struct is contiguous: class+0, sign+4, exp+8, frac+0xc
    uint32_t uStack_20 = rtguest::allocFrame(0x10);
  
  (*(uint32_t*)rtguest::G((uint32_t)uStack_20 + (uint32_t)(0) * 4)) = param_1;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_20 + (uint32_t)(1) * 4)) = param_2;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_20 + (uint32_t)(2) * 4)) = param_3;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_20 + (uint32_t)(3) * 4)) = param_4;
  RT_CALLRA(0x27d750, guest::__pack_f(uStack_20));
  return 0;
}


RT_GUEST_FN(0x0027d738, guest::__make_fp)

}  // namespace guest
