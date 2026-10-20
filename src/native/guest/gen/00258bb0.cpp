// guest:0x00258bb0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SndRpcCmd37(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
  // W13: tail-call passthrough (fn/00258bb0.c: void body = single call; v0 survives to the caller)
  return RT_CALLRA(0x258bdc, guest::sceSifCallRpc(0x175e840,0x37,0,0,0,0,0,0));
}


RT_GUEST_FN(0x00258bb0, guest::SndRpcCmd37)

}  // namespace guest
