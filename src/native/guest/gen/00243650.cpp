// guest:0x00243650 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetLabelEntry_f0810(uint32_t param_1, uint32_t param_2, ...) {
  return (*(uint32_t*)rtguest::G((uint32_t)(0x2f0810u + (uint32_t)(param_2 * 2) * 4)));
}


RT_GUEST_FN(0x00243650, guest::GetLabelEntry_f0810)

}  // namespace guest
