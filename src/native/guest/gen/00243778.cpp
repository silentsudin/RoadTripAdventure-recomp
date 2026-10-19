// guest:0x00243778 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetIntField_eff48(uint64_t param_1, uint32_t param_2, ...) {
  return (*(uint32_t*)rtguest::G((uint32_t)(0x002eff48 + param_2 * 0x10)));
}


RT_GUEST_FN(0x00243778, GetIntField_eff48)

}  // namespace guest
