// guest:0x0023d5e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t TestObjectAttribute(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  return (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + param_2 + 0xc))) == param_3;
}


RT_GUEST_FN(0x0023d5e0, guest::TestObjectAttribute)

}  // namespace guest
