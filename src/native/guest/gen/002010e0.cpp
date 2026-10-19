// guest:0x002010e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t switchD_0020101c_uiKanaGlyphUvCase81a0(uint32_t q0, ...) {
uint32_t in_t0_lo;
uint32_t in_t1_lo;
  
  (*(uint16_t*)rtguest::G((uint32_t)in_t1_lo)) = 200;
  (*(int16_t*)rtguest::G((uint32_t)in_t0_lo)) = (*(int16_t*)rtguest::G((uint32_t)in_t0_lo)) + 0xe0;
  return 0;
}


RT_GUEST_FN(0x002010e0, guest::switchD_0020101c_uiKanaGlyphUvCase81a0)

}  // namespace guest
