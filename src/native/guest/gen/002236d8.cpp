// guest:0x002236d8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t DrawCarVariant0(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
  // W13: tail-call passthrough (fn/002236d8.c: void body = single call; v0 survives to the caller)
  return drawCar(0,0,param_1,param_2,0);
}


RT_GUEST_FN(0x002236d8, guest::DrawCarVariant0)

}  // namespace guest
