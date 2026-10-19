// guest:0x00287928 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _sceRpcGetFPacket2(uint32_t param_1, uint32_t param_2, ...) {
  int iVar1;
  
  if ((param_2 < 0) || ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20))) <= param_2)) {
    iVar1 = _sceRpcGetFPacket();
  }
  else {
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) + param_2 * 0x40;
  }
}


RT_GUEST_FN(0x00287928, guest::_sceRpcGetFPacket2)

}  // namespace guest
