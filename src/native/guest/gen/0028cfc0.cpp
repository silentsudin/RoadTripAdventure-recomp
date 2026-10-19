// guest:0x0028cfc0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t supplement_crt0(uint32_t q0, ...) {
    uint32_t uStack_28 = rtguest::allocFrame(4);
    uint32_t uStack_2c = rtguest::allocFrame(4);
    uint32_t uStack_48 = rtguest::allocFrame(4);
    uint32_t uStack_4c = rtguest::allocFrame(4);
uint32_t auStack_50 = rtguest::allocFrame(4);
uint32_t auStack_30 = rtguest::allocFrame(4);
  
  (*(uint32_t*)rtguest::G((uint32_t)uStack_28)) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_4c)) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_48)) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_2c)) = 1;
  (*(uint32_t*)rtguest::G(0x2e99a8)) = CreateSema(auStack_50);
  (*(uint32_t*)rtguest::G(0x2e99ac)) = CreateSema(auStack_30);
  return 0;
}


RT_GUEST_FN(0x0028cfc0, guest::supplement_crt0)

}  // namespace guest
