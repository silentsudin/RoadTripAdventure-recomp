// guest:0x00208428 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t PlayTaskSound(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
  // W13: tail-call passthrough (fn/00208428.c: void body = single call; v0 survives to the caller)
  return PlayUiSound((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) + 0x8000);
}


RT_GUEST_FN(0x00208428, guest::PlayTaskSound)

}  // namespace guest
