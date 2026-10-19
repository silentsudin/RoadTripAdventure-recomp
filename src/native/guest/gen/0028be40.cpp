// guest:0x0028be40 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifFreeSysMemory(uint32_t q0, ...) {
  sceSifFreeIopHeap(q0);  // MIPS forwards its own $a0 into the free path (Ghidra dropped it)
  return 0;
}


RT_GUEST_FN(0x0028be40, guest::sceSifFreeSysMemory)

}  // namespace guest
