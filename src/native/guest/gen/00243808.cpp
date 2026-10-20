// guest:0x00243808 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetIntField_f00a8(uint64_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  return (*(uint32_t*)rtguest::G((uint32_t)(0x002f00a8 + param_2 * 0x10)));
}


RT_GUEST_FN(0x00243808, GetIntField_f00a8)

}  // namespace guest
