// guest:0x00270e88 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

void textGridEmitRows(uint32_t param_1, uint64_t param_2) {
  uint uVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  
  uVar3 = 0;
  uVar1 = (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t)));
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_1));
  iVar4 = (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int32_t)));
  if (uVar1 != 0) {
    do {
      textEmitGlyphsToGs(param_2,param_1 + 6,0,uVar3,iVar4,iVar2);
      uVar3 = uVar3 + 1;
      iVar4 = iVar4 + iVar2 * 2;
    } while (uVar3 < uVar1);
  }
  return;
}


RT_GUEST_FN(0x00270e88, guest::textGridEmitRows)

}  // namespace guest
