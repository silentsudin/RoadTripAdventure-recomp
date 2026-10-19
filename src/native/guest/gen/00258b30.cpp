// guest:0x00258b30 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SoundRpcCmd35(uint32_t q0, ...) {
  sceSifCallRpc(0x175e840,0x35,0,0,0,0,0,0);
  return 0;
}


RT_GUEST_FN(0x00258b30, guest::SoundRpcCmd35)

}  // namespace guest
