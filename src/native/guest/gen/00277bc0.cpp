// guest:0x00277bc0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// UB fix: pseudocode ulong copysign(ulong,long) (00277bc0.c); uint32 params dropped high words
uint64_t copysign(uint64_t param_1, int64_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  return param_1 & 0xffffffff |
         ((long)param_1 >> 0x20 & 0x7fffffffU | param_2 >> 0x20 & 0xffffffff80000000U) << 0x20;
}


RT_GUEST_FN(0x00277bc0, guest::copysign)

}  // namespace guest
