// guest:0x0028cdf0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifIsAliveIop(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
  ulong uVar1;
  
  uVar1 = RT_CALLRA(0x28cdf8, guest::sceSifGetReg(4));
  return (uVar1 & 0x10000) != 0;
}


RT_GUEST_FN(0x0028cdf0, guest::sceSifIsAliveIop)

}  // namespace guest
