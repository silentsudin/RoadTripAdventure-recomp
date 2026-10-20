// guest:0x002585b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SoundRpcStopAll(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  sceSifCallRpc(0x175e840,0x20,0,0,0,0,0,0);
  return 0;
}


RT_GUEST_FN(0x002585b0, guest::SoundRpcStopAll)

}  // namespace guest
