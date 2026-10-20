// guest:0x00243630 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetLabelEntry_f06c0(uint64_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  return (*(uint32_t*)rtguest::G((uint32_t)(0x2f06c0u + param_2 * 3 * 4)));
}


RT_GUEST_FN(0x00243630, GetLabelEntry_f06c0)

}  // namespace guest
