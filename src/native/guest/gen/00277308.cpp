// guest:0x00277308 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t fabs(uint32_t __x, ...) {
  rtguest::FrameGuard rt_fg;
  return (double)((uint32_t)__x & 0xffffffff | ((long)__x >> 0x20 & 0x7fffffffU) << 0x20);
}


RT_GUEST_FN(0x00277308, guest::fabs)

}  // namespace guest
