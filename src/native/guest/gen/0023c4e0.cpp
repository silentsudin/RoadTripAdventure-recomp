// guest:0x0023c4e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t switchD_0023c440_setObjectAttributeCase8(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(0);
  if (param_3 != 0) {
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 8))) = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 8))) | 0x10;
    return 0;
  }
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 8))) = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 8))) & 0xffef;
  return 0;
}


RT_GUEST_FN(0x0023c4e0, guest::switchD_0023c440_setObjectAttributeCase8)

}  // namespace guest
