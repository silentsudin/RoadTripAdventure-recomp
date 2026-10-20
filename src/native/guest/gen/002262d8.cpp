// guest:0x002262d8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetDrawSkyColours(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  char cVar1;
  byte bVar2;
  
  cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x22)));
  if (((cVar1 != -1) && (cVar1 != '\x11')) && (cVar1 != '\"')) {
    (*(float*)rtguest::G((uint32_t)param_1)) = 1.0;
    (*(float*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(float))) = 1.0;
    (*(float*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(float))) = 1.0;
    (*(float*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(float))) = 1.0;
    return 0;
  }
  (*(float*)rtguest::G((uint32_t)param_1)) = (float)(*(int8_t*)rtguest::G(0x177f754)) / 255.0;
  (*(float*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(float))) = (float)(*(uint8_t*)rtguest::G(0x177f755)) / 255.0;
  bVar2 = (*(uint8_t*)rtguest::G(0x177f756));
  (*(float*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(float))) = 1.0;
  (*(float*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(float))) = (float)bVar2 / 255.0;
  return 0;
}


RT_GUEST_FN(0x002262d8, guest::SetDrawSkyColours)

}  // namespace guest
