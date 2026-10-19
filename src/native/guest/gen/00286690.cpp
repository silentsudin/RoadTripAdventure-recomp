// guest:0x00286690 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sioPutCharCRLF(uint64_t param_1, ...) {
  if (param_1 == 10) {
    sioPutChar(0xd);
    sioPutChar(10);
    return 0;
  }
  sioPutChar(param_1);  // MIPS tail-calls with $4 = param_1 passed through (Ghidra dropped it)
  return 0;
}


RT_GUEST_FN(0x00286690, guest::sioPutCharCRLF)

}  // namespace guest
