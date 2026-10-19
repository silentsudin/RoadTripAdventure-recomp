// guest:0x0024f3b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CountSetBitsInByteArray(uint32_t param_1, uint32_t param_2, ...) {
uint32_t pbVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  
  iVar3 = 0;
  iVar4 = 0;
  pbVar1 = param_1;
  if (0 < param_2) {
    while( true ) {
      for (uVar2 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar1)); uVar2 != 0; uVar2 = uVar2 >> 1) {
        iVar4 = iVar4 + (uVar2 & 1);
      }
      iVar3 = iVar3 + 1;
      if (param_2 <= iVar3) break;
      pbVar1 = param_1 + iVar3;
    }
  }
  return iVar4;
}


RT_GUEST_FN(0x0024f3b8, guest::CountSetBitsInByteArray)

}  // namespace guest
