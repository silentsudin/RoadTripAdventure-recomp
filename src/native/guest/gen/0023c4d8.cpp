// guest:0x0023c4d8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t switchD_0023c440_setObjectAttributeCase7(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 6))) = param_3;
  return 0;
}


RT_GUEST_FN(0x0023c4d8, guest::switchD_0023c440_setObjectAttributeCase7)

}  // namespace guest
