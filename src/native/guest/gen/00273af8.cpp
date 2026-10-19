// guest:0x00273af8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t scePadGetFrameCount(uint32_t param_1, uint32_t param_2, ...) {
  int iVar1;
  undefined4 uVar2;
  
  uVar2 = 0;
  if ((*(uint32_t*)rtguest::G((uint32_t)0x1773220 + (uint32_t)((int)param_1 * 0x1c + param_2 * 7) * 4)) != 0) {
    iVar1 = scePadGetDmaStr(param_1);
    uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x58)));
  }
}


RT_GUEST_FN(0x00273af8, guest::scePadGetFrameCount)

}  // namespace guest
