// guest:0x00284710 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t isnan(uint32_t param_1, ...) {
  return 0x7ff00000 -
         ((uint32_t)(param_1 >> 0x20) & 0x7fffffff | ((uint32_t)param_1 | -(uint32_t)param_1) >> 0x1f) >>
         0x1f;
}


RT_GUEST_FN(0x00284710, guest::isnan)

}  // namespace guest
