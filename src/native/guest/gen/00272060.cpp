// guest:0x00272060 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

void textBufferBlendPixel16(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint16_t param_4, int16_t param_5) {
  rtguest::FrameGuard rt_fg;
  uint8_t bVar1;
uint32_t puVar2;
  
  param_4 = param_4 & 0xff;
  if ((((-1 < (int)param_2) && (param_2 < (*(uint32_t*)rtguest::G((uint32_t)param_1)))) && (-1 < (int)param_3)) &&
     (param_3 < (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint32_t))))) {
    puVar2 = (uint32_t)((param_3 * (*(uint32_t*)rtguest::G((uint32_t)param_1)) + param_2) * 2 + (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))));
    bVar1 = (uint8_t)(*(uint16_t*)rtguest::G((uint32_t)puVar2));
    if (0xf0 < bVar1) {
      param_4 = bVar1 | param_4;
    }
    (*(uint16_t*)rtguest::G((uint32_t)puVar2)) = param_4 | param_5 << 8;
  }
  return;
}


RT_GUEST_FN(0x00272060, guest::textBufferBlendPixel16)

}  // namespace guest
