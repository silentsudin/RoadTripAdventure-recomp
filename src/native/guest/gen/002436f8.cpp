// guest:0x002436f8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetIntField_ed330(uint64_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  return (*(uint32_t*)rtguest::G((uint32_t)(0x002ed330 + param_2 * 0x1c)));
}


RT_GUEST_FN(0x002436f8, GetIntField_ed330)

}  // namespace guest
