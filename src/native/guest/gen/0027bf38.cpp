// guest:0x0027bf38 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t dpcmp(uint64_t param_1, uint64_t param_2, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x40) = (uint32_t)(int32_t)param_1;
    *(uint32_t*)rtguest::G(rt_sv+0x44) = (uint32_t)((int64_t)(int32_t)param_1 >> 32);
    *(uint32_t*)rtguest::G(rt_sv+0x48) = (uint32_t)(int32_t)param_2;
    *(uint32_t*)rtguest::G(rt_sv+0x4c) = (uint32_t)((int64_t)(int32_t)param_2 >> 32);
  }
uint32_t auStack_60 = rtguest::allocFrame(32);
uint32_t auStack_40 = rtguest::allocFrame(32);
    uint32_t uStack_20 = rtguest::allocFrame(8);
    uint32_t uStack_18 = rtguest::allocFrame(8);
  
  (*(uint64_t*)rtguest::G((uint32_t)uStack_20)) = param_1;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_18)) = param_2;
  __unpack_d(uStack_20,auStack_60);
  __unpack_d(uStack_18,auStack_40);
  return __fpcmp_parts_d(auStack_60,auStack_40);  // v0 passthrough; callers use the result
}


RT_GUEST_FN(0x0027bf38, guest::dpcmp)

}  // namespace guest
