// guest:0x0028c950 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifLoadModule(uint32_t q0, ...) {
  _sceSifLoadModule();
  return 0;
}


RT_GUEST_FN(0x0028c950, guest::sceSifLoadModule)

}  // namespace guest
