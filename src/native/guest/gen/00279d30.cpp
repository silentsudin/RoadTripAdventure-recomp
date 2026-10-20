// guest:0x00279d30 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint64_t sceMcSeek(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(96);
  long lVar1;
  
  lVar1 = PollSema((*(uint32_t*)rtguest::G(0x2e89bc)));
  if (lVar1 < 0) {
    lVar1 = -200;
  }
  else if ((*(uint32_t*)rtguest::G(0x1774b64)) == 0) {
    SignalSema((*(uint32_t*)rtguest::G(0x2e89bc)));
    lVar1 = -100;
  }
  else {
    (*(uint32_t*)rtguest::G(0x1774bc0)) = param_1;
    (*(uint32_t*)rtguest::G(0x1774bd0)) = param_2;
    (*(uint32_t*)rtguest::G(0x1774bd4)) = param_3;
    lVar1 = sceSifCallRpc(0x1774b40,4,1,0x1774bc0,0x30,0x1776100,4,0);
    if (lVar1 == 0) {
      (*(uint32_t*)rtguest::G(0x2e89b8)) = 4;
    }
    else {
      SignalSema((*(uint32_t*)rtguest::G(0x2e89bc)));
    }
  }
  return lVar1;
}


RT_GUEST_FN(0x00279d30, guest::sceMcSeek)

}  // namespace guest
