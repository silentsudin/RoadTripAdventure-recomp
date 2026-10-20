// guest:0x00205f48 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawWidgetIfFlag16Alt(uint32_t param_1, uint64_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  if ((param_2 & 0x10) != 0) {
    uiDrawIcon20((int)(*(int16_t*)rtguest::G((uint32_t)param_1)) + ((int)((int)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int16_t))) + (uint32_t)((uint32_t)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int16_t))) >> 0xf)) >> 1) +
                 -10,((int)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int16_t))) + (int)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(int16_t)))) * 2,3);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x00205f48, guest::DrawWidgetIfFlag16Alt)

}  // namespace guest
