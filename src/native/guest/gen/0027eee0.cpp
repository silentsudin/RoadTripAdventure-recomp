// guest:0x0027eee0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sprintf(uint32_t param_1, uint32_t param_2, uint64_t param_3, uint64_t param_4, uint64_t param_5, uint64_t param_6, uint64_t param_7, uint64_t param_8, ...) {
uint32_t apuStack_c0 = rtguest::allocFrame(8);
    uint32_t uStack_b8 = rtguest::allocFrame(4);
    uint32_t uStack_b4 = rtguest::allocFrame(2);
    rtguest::allocFrame(6);  // guest frame gap
    uint32_t uStack_ac = rtguest::allocFrame(4);
    rtguest::allocFrame(120);  // guest frame gap
    uint32_t uStack_30 = rtguest::allocFrame(8);
    uint32_t uStack_28 = rtguest::allocFrame(8);
    uint32_t uStack_20 = rtguest::allocFrame(8);
    uint32_t uStack_18 = rtguest::allocFrame(8);
    uint32_t uStack_10 = rtguest::allocFrame(8);
    uint32_t uStack_8 = rtguest::allocFrame(8);
uint32_t puStack_b0;
uint32_t puStack_6c;
  
  (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = 0x7fffffff;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b8)) = 0x7fffffff;
  (*(uint16_t*)rtguest::G((uint32_t)uStack_b4)) = 0x208;
  puStack_6c = (*(uint32_t*)rtguest::G(0x002e8cc0));
  (*(uint32_t*)rtguest::G((uint32_t)apuStack_c0 + (uint32_t)(0) * 4)) = param_1;
  puStack_b0 = param_1;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_30)) = param_3;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_28)) = param_4;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_20)) = param_5;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_18)) = param_6;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_10)) = param_7;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_8)) = param_8;
  vfprintf(apuStack_c0,param_2,uStack_30);
  (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)apuStack_c0 + (uint32_t)(0) * 4)))) = 0;
  return 0;
}


RT_GUEST_FN(0x0027eee0, guest::sprintf)

}  // namespace guest
