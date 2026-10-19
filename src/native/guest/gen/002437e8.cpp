// guest:0x002437e8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetIntField_f0800(uint64_t param_1, uint32_t param_2, ...) {
  return (*(uint32_t*)rtguest::G((uint32_t)(0x002f0800 + param_2 * 0xc)));
}


RT_GUEST_FN(0x002437e8, GetIntField_f0800)

}  // namespace guest
