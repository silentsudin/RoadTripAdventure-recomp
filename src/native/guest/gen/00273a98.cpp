// guest:0x00273a98 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t scePadGetDmaStr(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  int iVar1;
  
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x01773210 + param_2 * 0x1c + param_1 * 0x70)));
  sceSifWriteBackDCache(iVar1,iVar1 + 0x100);
  return iVar1 + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x58))) < (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0xd8)))) * 0x80;
}


RT_GUEST_FN(0x00273a98, guest::scePadGetDmaStr)

}  // namespace guest
