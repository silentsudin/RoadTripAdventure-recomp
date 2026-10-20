// guest:0x0022e7c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawScoreCounter(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
  DrawDialogTextLine(0x1b8,0x1a,(*(uint32_t*)rtguest::G(0x177fdb4)),0x177fd9f);
  return 0;
}


RT_GUEST_FN(0x0022e7c0, guest::DrawScoreCounter)

}  // namespace guest
