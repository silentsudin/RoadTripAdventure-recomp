// guest:0x0028ce18 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifSyncIop(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
  bool bVar1;
  ulong uVar2;
  
  uVar2 = RT_CALLRA(0x28ce20, guest::sceSifGetReg(4));
  bVar1 = (uVar2 & 0x40000) != 0;
  if (bVar1) {
    RT_CALLRA(0x28ce38, guest::sceResetttyinit());
  }
  return bVar1;
}


RT_GUEST_FN(0x0028ce18, guest::sceSifSyncIop)

}  // namespace guest
