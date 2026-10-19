// guest:0x0021c880 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetGridCellIndex(uint32_t param_1, ...) {
  int iVar1;
  int iVar2;
  uint uVar3;
  
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int32_t)));
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)param_1));
  if (iVar2 < 0) {
    iVar2 = iVar2 + 0x1ffffff;
  }
  if (iVar1 < 0) {
    iVar1 = iVar1 + 0xffffff;
  }
  uVar3 = iVar2 >> 0x19 & 1;
  return ((iVar1 >> 0x18) - uVar3 & 2) + uVar3;
}


RT_GUEST_FN(0x0021c880, guest::GetGridCellIndex)

}  // namespace guest
