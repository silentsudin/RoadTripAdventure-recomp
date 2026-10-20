// guest:0x00234080 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetSlotConfig(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(0);
  param_1 = param_1 + param_3 * 0x14 + param_2 * 0x408 + 0x84f0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xc))) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x18))) = param_4;
  return 0;
}


RT_GUEST_FN(0x00234080, guest::SetSlotConfig)

}  // namespace guest
