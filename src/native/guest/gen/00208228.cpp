// guest:0x00208228 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t FindNthSetBit(uint32_t param_1, uint32_t param_2, ...) {
  uint uVar1;
  uint uVar2;
  
  uVar2 = 0;
  uVar1 = 1;
  while( true ) {
    if ((param_1 & uVar1) != 0) {
      if (param_2 == 0) {
      }
      param_2 = param_2 + -1;
    }
    uVar2 = uVar2 + 1;
    if (0x1f < (int)uVar2) break;
    uVar1 = 1 << (uVar2 & 0x1f);
  }
  return 0xffffffff;
}


RT_GUEST_FN(0x00208228, guest::FindNthSetBit)

}  // namespace guest
