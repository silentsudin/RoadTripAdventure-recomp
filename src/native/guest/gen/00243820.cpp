// guest:0x00243820 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetIntField_f06c8(uint64_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(0);
  return (*(uint32_t*)rtguest::G((uint32_t)(0x002f06c8 + param_2 * 0xc)));
}


RT_GUEST_FN(0x00243820, GetIntField_f06c8)

}  // namespace guest
