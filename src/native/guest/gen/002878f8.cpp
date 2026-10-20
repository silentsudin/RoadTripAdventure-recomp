// guest:0x002878f8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _sceRpcGetFPacket(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x24))) % (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x18)));
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x18))) == 0) {
    trap(7);
  }
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x24))) = iVar1 + 1;
  return (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x14))) + iVar1 * 0x40;
}


RT_GUEST_FN(0x002878f8, guest::_sceRpcGetFPacket)

}  // namespace guest
