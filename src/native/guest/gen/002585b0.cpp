// guest:0x002585b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SoundRpcStopAll(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
  // W13: tail-call passthrough (fn/002585b0.c: void body = single call; v0 survives to the caller)
  return RT_CALLRA(0x2585dc, guest::sceSifCallRpc(0x175e840,0x20,0,0,0,0,0,0));
}


RT_GUEST_FN(0x002585b0, guest::SoundRpcStopAll)

}  // namespace guest
