// guest:0x0020e4e8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ToggleInputFlagWrapper(uint64_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
  // W13: tail-call passthrough (fn/0020e4e8.c: void body = single call; v0 survives to the caller)
  return debugScreenRead(param_1 ^ 1);
}


RT_GUEST_FN(0x0020e4e8, guest::ToggleInputFlagWrapper)

}  // namespace guest
