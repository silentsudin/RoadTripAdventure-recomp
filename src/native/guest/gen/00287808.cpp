// guest:0x00287808 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifExitRpc(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
  RT_CALLRA(0x287810, guest::sceSifExitCmd());
  (*(uint32_t*)rtguest::G(0x2e9900)) = 0;
  return 0;
}


RT_GUEST_FN(0x00287808, guest::sceSifExitRpc)

}  // namespace guest
