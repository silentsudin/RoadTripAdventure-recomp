// guest:0x00201100 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t switchD_0020101c_uiKanaGlyphUvCase817d(uint32_t q0, ...) {
uint32_t in_t0_lo;
uint32_t in_t1_lo;
  
  (*(uint16_t*)rtguest::G((uint32_t)in_t1_lo)) = 0xdc;
  (*(int16_t*)rtguest::G((uint32_t)in_t0_lo)) = (*(int16_t*)rtguest::G((uint32_t)in_t0_lo)) + 0x140;
  return 0;
}


RT_GUEST_FN(0x00201100, guest::switchD_0020101c_uiKanaGlyphUvCase817d)

}  // namespace guest
