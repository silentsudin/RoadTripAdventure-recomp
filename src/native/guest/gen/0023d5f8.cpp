// guest:0x0023d5f8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t TestStampFlag(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(0);
  if (param_2 < 0x20) {
    return (*(uint32_t*)rtguest::G((uint32_t)0x1780338 + (uint32_t)(param_1) * 4)) & 1 << ((uint32_t)param_2 & 0x1f);
  }
  return 0;
}


RT_GUEST_FN(0x0023d5f8, guest::TestStampFlag)

}  // namespace guest
