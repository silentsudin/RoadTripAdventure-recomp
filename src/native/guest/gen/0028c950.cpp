// guest:0x0028c950 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifLoadModule(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
  // W13: tail-call passthrough (fn/0028c950.c: void body = single call; v0 survives to the caller)
  return RT_CALLRA(0x28c95c, guest::_sceSifLoadModule());
}


RT_GUEST_FN(0x0028c950, guest::sceSifLoadModule)

}  // namespace guest
