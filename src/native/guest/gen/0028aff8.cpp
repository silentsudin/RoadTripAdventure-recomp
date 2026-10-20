// guest:0x0028aff8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceChdir(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
  // W13: tail-call passthrough (fn/0028aff8.c: void body = single call; v0 survives to the caller)
  return RT_CALLRA(0x28b000, guest::_sceCallCode(param_1,0x12));
}


RT_GUEST_FN(0x0028aff8, guest::sceChdir)

}  // namespace guest
