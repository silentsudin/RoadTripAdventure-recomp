// guest:0x00223810 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint64_t MakeVifCall15xGif(uint32_t param_1, ...) {
  return (long)(param_1 * 10 + 0x1be4) | 0x19308000U | (long)(param_1 * 10 + 0x1bec) << 0x25 |
         0x2010000540000000;
}


RT_GUEST_FN(0x00223810, guest::MakeVifCall15xGif)

}  // namespace guest
