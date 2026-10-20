// guest:0x002833f8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t setlocale(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
  // W13: tail-call passthrough (fn/002833f8.c: void body = single call; v0 survives to the caller)
  return _setlocale_r((*(uint32_t*)rtguest::G(0x002e8cc0)),param_1,param_2);
}


RT_GUEST_FN(0x002833f8, guest::setlocale)

}  // namespace guest
