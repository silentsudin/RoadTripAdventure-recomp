// guest:0x002436a0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetIntField_eca78(uint32_t param_1, uint32_t param_2, ...) {
  return (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002eca78 + param_2 * 0x1c)));
}


RT_GUEST_FN(0x002436a0, guest::GetIntField_eca78)

}  // namespace guest
