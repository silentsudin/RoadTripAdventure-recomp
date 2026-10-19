// guest:0x00243758 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetIntField_effe0(uint32_t param_1, uint32_t param_2, ...) {
  return (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002effe0 + param_2 * 0xc)));
}


RT_GUEST_FN(0x00243758, guest::GetIntField_effe0)

}  // namespace guest
