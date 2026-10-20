// guest:0x00257760 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t BindAndCallRpcInit(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  sceSifBindRpc(0x175e840,0x1234567,0);
  sceSifCallRpc(0x175e840,0,0,0,0,0,0,0);
  return 0;
}


RT_GUEST_FN(0x00257760, guest::BindAndCallRpcInit)

}  // namespace guest
