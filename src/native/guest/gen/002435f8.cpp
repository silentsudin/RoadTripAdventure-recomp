// guest:0x002435f8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetLabelEntry_f07f8(uint64_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(0);
  return (*(uint32_t*)rtguest::G((uint32_t)(0x2f07f8u + param_2 * 3 * 4)));
}


RT_GUEST_FN(0x002435f8, GetLabelEntry_f07f8)

}  // namespace guest
