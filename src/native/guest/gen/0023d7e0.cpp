// guest:0x0023d7e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ClearFlagBitFc98(uint64_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  (*(uint64_t*)rtguest::G((uint32_t)0x177fc98 + (uint32_t)(((uint32_t)param_1 & 0xff) >> 6) * 8)) =
       (*(uint64_t*)rtguest::G((uint32_t)0x177fc98 + (uint32_t)(((uint32_t)param_1 & 0xff) >> 6) * 8)) & ~(1L << (param_1 & 0x3f));
  return 0;
}


RT_GUEST_FN(0x0023d7e0, guest::ClearFlagBitFc98)

}  // namespace guest
