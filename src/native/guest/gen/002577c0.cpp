// guest:0x002577c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CallRpcCmd1(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(32);
  sceSifCallRpc(0x175e840,1,0,0,0,0,0,0);
  return 0;
}


RT_GUEST_FN(0x002577c0, guest::CallRpcCmd1)

}  // namespace guest
