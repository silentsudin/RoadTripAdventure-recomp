// guest:0x002436e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetIntField_ed200(uint32_t param_1, uint32_t param_2, ...) {
  return (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed200 + param_2 * 0x10)));
}


RT_GUEST_FN(0x002436e0, guest::GetIntField_ed200)

}  // namespace guest
