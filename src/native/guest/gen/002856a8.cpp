// guest:0x002856a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sysIsatty(uint32_t q0, ...) {
  return 1;
}


RT_GUEST_FN(0x002856a8, guest::sysIsatty)

}  // namespace guest
