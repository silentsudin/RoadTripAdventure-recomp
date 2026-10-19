// guest:0x0023d8c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetOrSetPresidentRaceResult(uint32_t param_1, ...) {
  if (param_1 != -1) {
    (*(uint32_t*)rtguest::G(0x335704)) = param_1;
    return param_1;
  }
  return (*(uint32_t*)rtguest::G(0x335704));
}


RT_GUEST_FN(0x0023d8c8, guest::GetOrSetPresidentRaceResult)

}  // namespace guest
