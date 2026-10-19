// guest:0x002848a0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __sclose(uint32_t param_1, ...) {
  _close_r((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x54))),(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xe))));
  return 0;
}


RT_GUEST_FN(0x002848a0, guest::__sclose)

}  // namespace guest
