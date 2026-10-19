// guest:0x00243588 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetLabelEntry_eff40(uint64_t param_1, uint32_t param_2, ...) {
  return (*(uint32_t*)rtguest::G((uint32_t)(0x2eff40u + param_2 * 4 * 4)));
}


RT_GUEST_FN(0x00243588, GetLabelEntry_eff40)

}  // namespace guest
