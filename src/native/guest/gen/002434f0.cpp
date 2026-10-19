// guest:0x002434f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#define GetLabelEntry_ed1f8 GetLabelEntry_ed1f8_decl
#include "guest_decls.inc"
#undef GetLabelEntry_ed1f8

namespace guest {

uint32_t GetLabelEntry_ed1f8(uint64_t param_1, uint32_t param_2, ...) {
  return (*(uint32_t*)rtguest::G((uint32_t)(0x2ed1f8u + (uint32_t)(param_2 * 4) * 4)));
}


RT_GUEST_FN(0x002434f0, guest::GetLabelEntry_ed1f8)

}  // namespace guest
