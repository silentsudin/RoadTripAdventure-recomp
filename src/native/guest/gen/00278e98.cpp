// guest:0x00278e98 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceCdMmode(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x40) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x44) = 0x00000000u;
  }
  long lVar1;
  
  lVar1 = RT_CALLRA(0x278eb8, guest::_sceCd_scmd_prechk(0x22));
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G(0x2e8540)) = param_1;
    sceSifWriteBackDCache(0x2e8540,4);
    lVar1 = RT_CALLRA(0x278f08, guest::sceSifCallRpc(0x2e8948,0x22,0,0x2e8540,4,0x2e8100,4,0));
    if (lVar1 < 0) {
      RT_CALLRA(0x278f1c, guest::SignalSema((*(int32_t*)rtguest::G(0x2e6f2c))));
    }
    else {
      RT_CALLRA(0x278f38, guest::SignalSema((*(int32_t*)rtguest::G(0x2e6f2c))));
    }
  }
  return 0;
}


RT_GUEST_FN(0x00278e98, guest::sceCdMmode)

}  // namespace guest
