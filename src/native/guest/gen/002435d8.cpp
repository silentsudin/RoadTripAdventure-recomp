// guest:0x002435d8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetLabelEntry_f03e0(uint64_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  return (*(uint32_t*)rtguest::G((uint32_t)(0x2f03e0u + param_2 * 5 * 4)));
}


RT_GUEST_FN(0x002435d8, GetLabelEntry_f03e0)

}  // namespace guest
