// guest:0x0027b7b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t fptodp(uint32_t param_1, ...) {
    uint32_t uStack_24 = rtguest::allocFrame(4);
    uint32_t uStack_28 = rtguest::allocFrame(4);
    uint32_t uStack_2c = rtguest::allocFrame(4);
    uint32_t uStack_30 = rtguest::allocFrame(4);
uint32_t auStack_20 = rtguest::allocFrame(16);
  
  (*(uint32_t*)rtguest::G((uint32_t)auStack_20 + (uint32_t)(0) * 4)) = param_1;
  __unpack_f(auStack_20,uStack_30);
  __make_dp((*(uint32_t*)rtguest::G((uint32_t)uStack_30)),(*(uint32_t*)rtguest::G((uint32_t)uStack_2c)),(*(uint32_t*)rtguest::G((uint32_t)uStack_28)),(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)uStack_24)) << 0x1e);
  return 0;
}


RT_GUEST_FN(0x0027b7b0, guest::fptodp)

}  // namespace guest
