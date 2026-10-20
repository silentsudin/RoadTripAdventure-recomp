// guest:0x002434b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetLabelEntry_eca70(uint64_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  return (*(uint32_t*)rtguest::G((uint32_t)(0x2eca70u + (uint32_t)(param_2 * 7) * 4)));
}


RT_GUEST_FN(0x002434b0, guest::GetLabelEntry_eca70)

}  // namespace guest
