// guest:0x002435a0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetLabelEntry_f0280(uint32_t param_1, uint32_t param_2, ...) {
  return (*(uint32_t*)rtguest::G((uint32_t)(0x2f0280u + (uint32_t)(param_2 * 3) * 4)));
}


RT_GUEST_FN(0x002435a0, guest::GetLabelEntry_f0280)

}  // namespace guest
