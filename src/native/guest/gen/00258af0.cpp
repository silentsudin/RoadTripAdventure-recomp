// guest:0x00258af0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SoundRpcCmd34(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(32);
  sceSifCallRpc(0x175e840,0x34,0,0,0,0,0,0);
  return 0;
}


RT_GUEST_FN(0x00258af0, guest::SoundRpcCmd34)

}  // namespace guest
