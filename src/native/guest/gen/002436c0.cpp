// guest:0x002436c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetIntField_eced8(uint32_t param_1, uint32_t param_2, ...) {
  return (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002eced8 + param_2 * 0x18)));
}


RT_GUEST_FN(0x002436c0, guest::GetIntField_eced8)

}  // namespace guest
