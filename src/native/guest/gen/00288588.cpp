// guest:0x00288588 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifRpcLoop(uint64_t param_1, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
  long lVar1;
  
  do {
    while (lVar1 = sceSifGetNextRequest(param_1), lVar1 == 0) {
      RT_CALLRA(0x2885b8, guest::SleepThread());
    }
    RT_CALLRA(0x2885a0, guest::sceSifExecRequest(lVar1));
  } while( true );
}


RT_GUEST_FN(0x00288588, guest::sceSifRpcLoop)

}  // namespace guest
