// guest:0x002495f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t FormatRaceTimeBCD(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x14))) / 0x96;
  iVar2 = iVar1 / 10;
  iVar3 = iVar2 / 6;
  return iVar1 % 10 | iVar2 % 6 << 4 | 0xa00U | iVar3 % 10 << 0xc | (iVar3 / 10) % 10 << 0x10;
}


RT_GUEST_FN(0x002495f0, guest::FormatRaceTimeBCD)

}  // namespace guest
