// guest:0x0023dfe0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t HasQueuedByteFlag(uint32_t param_1, ...) {
  return (*(uint8_t*)rtguest::G((uint32_t)0x177fd08 + (uint32_t)(param_1 & 0xff) * 1)) & 0x80;
}


RT_GUEST_FN(0x0023dfe0, guest::HasQueuedByteFlag)

}  // namespace guest
