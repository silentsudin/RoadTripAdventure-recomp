// guest:0x00272010 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t textBufferGetPixel16(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  if ((((-1 < (int)param_2) && (param_2 < (*(uint32_t*)rtguest::G((uint32_t)param_1)))) && (-1 < (int)param_3)) &&
     (param_3 < (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint32_t))))) {
    return (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((param_3 * (*(uint32_t*)rtguest::G((uint32_t)param_1)) + param_2) * 2 + (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))))));
  }
  return 0x720;
}


RT_GUEST_FN(0x00272010, guest::textBufferGetPixel16)

}  // namespace guest
