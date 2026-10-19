// guest:0x0028d078 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceDeci2Close(uint32_t param_1, ...) {
uint32_t auStack_20 = rtguest::allocFrame(16);
  
  (*(uint32_t*)rtguest::G((uint32_t)auStack_20 + (uint32_t)(0) * 4)) = param_1;
  Deci2Call(2,auStack_20);
  return 0;
}


RT_GUEST_FN(0x0028d078, guest::sceDeci2Close)

}  // namespace guest
