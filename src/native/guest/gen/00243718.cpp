// guest:0x00243718 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetIntField_ed4f0(uint32_t param_1, uint32_t param_2, ...) {
  return (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed4f0 + param_2 * 0x10)));
}


RT_GUEST_FN(0x00243718, guest::GetIntField_ed4f0)

}  // namespace guest
