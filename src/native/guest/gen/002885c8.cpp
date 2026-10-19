// guest:0x002885c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t fsFdSemInit(uint32_t q0, ...) {
    uint32_t uStack_2c = rtguest::allocFrame(4);
    uint32_t uStack_38 = rtguest::allocFrame(4);
    uint32_t uStack_3c = rtguest::allocFrame(4);
uint32_t auStack_40 = rtguest::allocFrame(4);
  
  if ((*(int32_t*)rtguest::G(0x2e9990)) == -1) {
    (*(uint32_t*)rtguest::G((uint32_t)uStack_2c)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_3c)) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_38)) = 1;
    (*(int32_t*)rtguest::G(0x2e9990)) = CreateSema(auStack_40);
    (*(uint32_t*)rtguest::G(0x2e9994)) = CreateSema(auStack_40);
  }
  return 0;
}


RT_GUEST_FN(0x002885c8, guest::fsFdSemInit)

}  // namespace guest
