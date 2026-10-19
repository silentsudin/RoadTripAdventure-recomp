// guest:0x00204ed0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetTaskFlag(uint32_t param_1, ...) {
  (*(int8_t*)rtguest::G((uint32_t)param_1)) = (*(int8_t*)rtguest::G((uint32_t)param_1)) | 2;
  return 0;
}


RT_GUEST_FN(0x00204ed0, guest::SetTaskFlag)

}  // namespace guest
