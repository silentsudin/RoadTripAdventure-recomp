// guest:0x0026e3e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceGsGetEnv(uint32_t q0, ...) {
  return 0x002dcd80;
}


RT_GUEST_FN(0x0026e3e0, guest::sceGsGetEnv)

}  // namespace guest
