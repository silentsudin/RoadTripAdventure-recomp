// guest:0x002855e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sysClose(uint32_t q0, ...) {
  return 0xffffffff;
}


RT_GUEST_FN(0x002855e0, guest::sysClose)

}  // namespace guest
