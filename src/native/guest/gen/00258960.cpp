// guest:0x00258960 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SndRpcCmd30(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(32);
  sceSifCallRpc(0x175e840,0x30,0,0,0,0,0,0);
  return 0;
}


RT_GUEST_FN(0x00258960, guest::SndRpcCmd30)

}  // namespace guest
