// guest:0x00249670 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetRaceTimeRank(uint32_t param_1, ...) {
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x14)));
  uVar2 = 0;
  if ((((0xe483 < iVar1) && (uVar2 = 1, 80999 < iVar1)) && (0x2327f < iVar1)) &&
     (uVar2 = 2, 0x278d0 < iVar1)) {
    uVar2 = 0;
  }
  return uVar2;
}


RT_GUEST_FN(0x00249670, guest::GetRaceTimeRank)

}  // namespace guest
