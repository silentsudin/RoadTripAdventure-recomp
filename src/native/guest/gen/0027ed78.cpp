// guest:0x0027ed78 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t signal(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
  // W13: tail-call passthrough (fn/0027ed78.c: void body = single call; v0 survives to the caller)
  return _signal_r((*(uint32_t*)rtguest::G(0x002e8cc0)),param_1,param_2);
}


RT_GUEST_FN(0x0027ed78, guest::signal)

}  // namespace guest
