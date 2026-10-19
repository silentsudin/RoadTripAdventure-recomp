// guest:0x002435c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetLabelEntry_f02f8(uint32_t param_1, uint32_t param_2, ...) {
  return (*(uint32_t*)rtguest::G((uint32_t)(0x2f02f8u + (uint32_t)(param_2 * 4) * 4)));
}


RT_GUEST_FN(0x002435c0, guest::GetLabelEntry_f02f8)

}  // namespace guest
