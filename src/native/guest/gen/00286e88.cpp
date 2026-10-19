// guest:0x00286e88 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t printf__00286e88(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, uint32_t param_7, uint32_t param_8, ...) {
    uint32_t uStack_10 = rtguest::allocFrame(8);
    uint32_t uStack_18 = rtguest::allocFrame(8);
    uint32_t uStack_20 = rtguest::allocFrame(8);
    uint32_t uStack_28 = rtguest::allocFrame(8);
    uint32_t uStack_30 = rtguest::allocFrame(8);
    uint32_t uStack_38 = rtguest::allocFrame(8);
    uint32_t uStack_8 = rtguest::allocFrame(8);
  
  (*(uint64_t*)rtguest::G((uint32_t)uStack_38)) = param_2;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_30)) = param_3;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_28)) = param_4;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_20)) = param_5;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_18)) = param_6;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_10)) = param_7;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_8)) = param_8;
  _printf(param_1,uStack_38);
  return 0;
}


RT_GUEST_FN(0x00286e88, guest::printf__00286e88)

}  // namespace guest
