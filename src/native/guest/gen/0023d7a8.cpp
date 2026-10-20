// guest:0x0023d7a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetFlagBitFc98(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  (*(uint64_t*)rtguest::G((uint32_t)0x177fc98 + (uint32_t)(((uint32_t)param_1 & 0xff) >> 6) * 8)) =
       (*(uint64_t*)rtguest::G((uint32_t)0x177fc98 + (uint32_t)(((uint32_t)param_1 & 0xff) >> 6) * 8)) | 1L << (param_1 & 0x3f);
  return 0;
}


RT_GUEST_FN(0x0023d7a8, guest::SetFlagBitFc98)

}  // namespace guest
