// guest:0x002080f8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiFade(uint64_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
  (*(uint64_t*)rtguest::G(0x29abb0)) = param_1;
  vifDirect(0x29ab70,6);
  return 0;
}


RT_GUEST_FN(0x002080f8, guest::uiFade)

}  // namespace guest
