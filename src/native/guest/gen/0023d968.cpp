// guest:0x0023d968 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CountBitsFca8Fcb0(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(0);
  ulong uVar1;
  int iVar2;
  
  iVar2 = 0;
  for (uVar1 = (*(uint64_t*)rtguest::G((uint32_t)0x177fca8 + (uint32_t)(param_1 * 0x689) * 8)); uVar1 != 0; uVar1 = uVar1 >> 1) {
    iVar2 = iVar2 + ((uint32_t)uVar1 & 1);
  }
  for (uVar1 = (*(uint64_t*)rtguest::G((uint32_t)0x177fcb0 + (uint32_t)(param_1 * 0x689) * 8)); uVar1 != 0; uVar1 = uVar1 >> 1) {
    iVar2 = iVar2 + ((uint32_t)uVar1 & 1);
  }
  return iVar2;
}


RT_GUEST_FN(0x0023d968, guest::CountBitsFca8Fcb0)

}  // namespace guest
