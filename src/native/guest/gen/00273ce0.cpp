// guest:0x00273ce0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t scePadGetReqState(uint64_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  undefined1 uVar1;
  int iVar2;
  
  uVar1 = 0;
  if ((*(uint32_t*)rtguest::G((uint32_t)0x1773220 + (uint32_t)((int)param_1 * 0x1c + param_2 * 7) * 4)) != 0) {
    iVar2 = scePadGetDmaStr(param_1);
    uVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x71)));
  }
  return uVar1;
}


RT_GUEST_FN(0x00273ce0, guest::scePadGetReqState)

}  // namespace guest
