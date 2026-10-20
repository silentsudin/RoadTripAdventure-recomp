// guest:0x00271fc0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

void textBufferSetPixel16(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint16_t param_4) {
  rtguest::FrameGuard rt_fg;
  if ((((-1 < (int)param_2) && (param_2 < (*(uint32_t*)rtguest::G((uint32_t)param_1)))) && (-1 < (int)param_3)) &&
     (param_3 < (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint32_t))))) {
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((param_3 * (*(uint32_t*)rtguest::G((uint32_t)param_1)) + param_2) * 2 + (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t)))))) = param_4;
  }
  return;
}


RT_GUEST_FN(0x00271fc0, guest::textBufferSetPixel16)

}  // namespace guest
