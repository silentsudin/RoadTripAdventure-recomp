// guest:0x002855b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sysStubEio(uint32_t q0, ...) {
uint32_t puVar1;
  
  puVar1 = (uint32_t)getReent();
  (*(uint32_t*)rtguest::G((uint32_t)puVar1)) = 5;
  return 0xffffffff;
}


RT_GUEST_FN(0x002855b8, guest::sysStubEio)

}  // namespace guest
