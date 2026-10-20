// guest:0x002577c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CallRpcCmd1(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
  // W13: tail-call passthrough (fn/002577c0.c: void body = single call; v0 survives to the caller)
  return RT_CALLRA(0x2577ec, guest::sceSifCallRpc(0x175e840,1,0,0,0,0,0,0));
}


RT_GUEST_FN(0x002577c0, guest::CallRpcCmd1)

}  // namespace guest
