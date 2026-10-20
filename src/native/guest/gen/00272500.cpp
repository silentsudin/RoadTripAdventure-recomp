// guest:0x00272500 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gifPkBufferReset(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(0);
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint32_t)));
  return 0;
}


RT_GUEST_FN(0x00272500, guest::gifPkBufferReset)

}  // namespace guest
