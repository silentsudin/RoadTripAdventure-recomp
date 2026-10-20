// guest:0x00205c08 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiMenuRun(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
  // W13: tail jump (fn/00205c08.c: "tail-jumps to uiMenuWindow (0x205350) with a2=0") — v0 passes through to the caller.
  return uiMenuWindow(param_1,param_2,0);
}


RT_GUEST_FN(0x00205c08, guest::uiMenuRun)

}  // namespace guest
