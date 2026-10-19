// guest:0x0023de30 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t TestProgressFlag(uint32_t param_1, ...) {
  return ((*(uint32_t*)rtguest::G(0x177fdd0)) & 1 << (param_1 & 0x1f)) != 0;
}


RT_GUEST_FN(0x0023de30, guest::TestProgressFlag)

}  // namespace guest
