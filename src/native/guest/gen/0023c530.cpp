// guest:0x0023c530 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t switchD_0023c440_setObjectAttrFlagGroup10(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg;
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 8))) =
       (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 8))) & 0xdfbf | (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x002f0304 + param_3 * 0x10)));
  return 0;
}


RT_GUEST_FN(0x0023c530, guest::switchD_0023c440_setObjectAttrFlagGroup10)

}  // namespace guest
