// guest:0x00201088 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t switchD_0020101c_uiKanaGlyphUvCase8175(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
uint32_t in_t0_lo;
uint32_t in_t1_lo;
  
  (*(uint16_t*)rtguest::G((uint32_t)in_t1_lo)) = 0x1e0;
  (*(int16_t*)rtguest::G((uint32_t)in_t0_lo)) = (*(int16_t*)rtguest::G((uint32_t)in_t0_lo)) + 0xa0;
  return 0;
}


RT_GUEST_FN(0x00201088, guest::switchD_0020101c_uiKanaGlyphUvCase8175)

}  // namespace guest
