// guest:0x0027c0e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __make_dp(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
    uint32_t uStack_20 = rtguest::allocFrame(8);
    uint32_t uStack_28 = rtguest::allocFrame(4);
    uint32_t uStack_2c = rtguest::allocFrame(4);
    uint32_t uStack_30 = rtguest::allocFrame(4);
  
  (*(uint32_t*)rtguest::G((uint32_t)uStack_30)) = param_1;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_2c)) = param_2;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_28)) = param_3;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_20)) = param_4;
  __pack_d(uStack_30);
  return 0;
}


RT_GUEST_FN(0x0027c0e0, guest::__make_dp)

}  // namespace guest
