// guest:0x0020e018 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ClearInputTransientState(uint32_t param_1, ...) {
  if ((*(uint32_t*)rtguest::G(0x333370)) == 0) {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xf))) = 0;
  }
  return 0;
}


RT_GUEST_FN(0x0020e018, guest::ClearInputTransientState)

}  // namespace guest
