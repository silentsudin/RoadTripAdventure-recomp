// guest:0x0027bf38 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#define dpcmp dpcmp_decl
#include "guest_decls.inc"
#undef dpcmp

namespace guest {

uint32_t dpcmp(uint64_t param_1, uint64_t param_2, ...) {
    uint32_t uStack_18 = rtguest::allocFrame(8);
    uint32_t uStack_20 = rtguest::allocFrame(8);
uint32_t auStack_60 = rtguest::allocFrame(32);
uint32_t auStack_40 = rtguest::allocFrame(32);
  
  (*(uint64_t*)rtguest::G((uint32_t)uStack_20)) = param_1;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_18)) = param_2;
  __unpack_d(uStack_20,auStack_60);
  __unpack_d(uStack_18,auStack_40);
  __fpcmp_parts_d(auStack_60,auStack_40);
  return 0;
}


RT_GUEST_FN(0x0027bf38, guest::dpcmp)

}  // namespace guest
