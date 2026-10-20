// guest:0x002724f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gifPkBufferInit(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  (*(uint32_t*)rtguest::G((uint32_t)param_1)) = param_2;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint32_t))) = param_2;
  return 0;
}


RT_GUEST_FN(0x002724f0, guest::gifPkBufferInit)

}  // namespace guest
