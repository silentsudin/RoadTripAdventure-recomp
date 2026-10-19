// guest:0x0021c8d0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ComputeGridCellIndex(uint32_t param_1, uint32_t param_2, ...) {
  uint uVar1;
  
  if (param_2 < 0) {
    param_2 = param_2 + 0x1ffffff;
  }
  if (param_1 < 0) {
    param_1 = param_1 + 0xffffff;
  }
  uVar1 = param_2 >> 0x19 & 1;
  return (param_2 >> 0x19 & 6U) * 8 + ((param_1 >> 0x18) - uVar1 & 0xe) + uVar1;
}


RT_GUEST_FN(0x0021c8d0, guest::ComputeGridCellIndex)

}  // namespace guest
