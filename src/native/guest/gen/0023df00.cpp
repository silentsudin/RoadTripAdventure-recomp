// guest:0x0023df00 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetQueuedByte(uint32_t param_1, ...) {
  if ((int)(param_1 & 0xff) < (*(int32_t*)rtguest::G(0x177fd88))) {
    return (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)0x177fd08 + (uint32_t)(param_1 & 0xff) * 1)) & 0x7f;
  }
  return 0xffffffff;
}


RT_GUEST_FN(0x0023df00, guest::GetQueuedByte)

}  // namespace guest
