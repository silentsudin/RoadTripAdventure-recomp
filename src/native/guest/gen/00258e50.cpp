// guest:0x00258e50 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t PlayUiSound(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
  // W13: tail-call passthrough (fn/00258e50.c: void body = single call; v0 survives to the caller)
  return SndRpcPlayOrStop(param_1 | 0x40400000);
}


RT_GUEST_FN(0x00258e50, guest::PlayUiSound)

}  // namespace guest
