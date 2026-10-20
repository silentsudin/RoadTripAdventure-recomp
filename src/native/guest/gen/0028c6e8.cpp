// guest:0x0028c6e8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifLoadModuleBuffer(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
  RT_CALLRA(0x28c6f0, guest::_sceSifLoadModuleBuffer());
  return 0;
}


RT_GUEST_FN(0x0028c6e8, guest::sceSifLoadModuleBuffer)

}  // namespace guest
