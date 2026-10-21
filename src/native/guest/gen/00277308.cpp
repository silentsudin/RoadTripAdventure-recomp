// guest:0x00277308 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint64_t fabs(uint64_t __x, ...) {  // double as bits (pseudocode 0x277308: double fabs(double))
  rtguest::FrameGuard rt_fg(0);
  return (uint64_t)((uint32_t)__x & 0xffffffff | ((long)__x >> 0x20 & 0x7fffffffU) << 0x20);
}


RT_GUEST_FN(0x00277308, guest::fabs)

}  // namespace guest
