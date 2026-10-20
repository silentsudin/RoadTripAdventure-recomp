// guest:0x0027b778 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __main(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
  if ((*(int32_t*)rtguest::G(0x1776158)) == 0) {
    (*(int32_t*)rtguest::G(0x1776158)) = 1;
    __do_global_ctors();
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x0027b778, guest::__main)

}  // namespace guest
