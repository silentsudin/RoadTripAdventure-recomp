// guest:0x0027d738 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __make_fp(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
    uint32_t uStack_14 = rtguest::allocFrame(4);
    uint32_t uStack_18 = rtguest::allocFrame(4);
    uint32_t uStack_1c = rtguest::allocFrame(4);
    uint32_t uStack_20 = rtguest::allocFrame(4);
  
  (*(uint32_t*)rtguest::G((uint32_t)uStack_20)) = param_1;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_1c)) = param_2;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_18)) = param_3;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_14)) = param_4;
  __pack_f(uStack_20);
  return 0;
}


RT_GUEST_FN(0x0027d738, guest::__make_fp)

}  // namespace guest
