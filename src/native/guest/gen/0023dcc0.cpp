// guest:0x0023dcc0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t TestFlagFcb8(uint32_t param_1, uint32_t param_2, ...) {
  int iVar1;
  
  iVar1 = 9;
  if (param_1 != 0xd) {
    iVar1 = (int)param_1 + -1;
  }
  return ((*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x0177fcb8 + iVar1 * 8))) & 1L << param_2) != 0;
}


RT_GUEST_FN(0x0023dcc0, guest::TestFlagFcb8)

}  // namespace guest
