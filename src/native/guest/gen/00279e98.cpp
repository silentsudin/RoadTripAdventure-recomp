// guest:0x00279e98 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint64_t sceMcRead(uint32_t param_1, uint64_t param_2, uint64_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
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
    (*(uint32_t*)rtguest::G(0x1774bdc)) = 0x01775040;
    (*(uint32_t*)rtguest::G(0x1774bd8)) = (uint32_t)param_2;
    (*(uint32_t*)rtguest::G(0x1774bcc)) = (uint32_t)param_3;
    (*(uint32_t*)rtguest::G(0x1774bc0)) = param_1;
    sceSifWriteBackDCache(param_2,param_3);
    sceSifWriteBackDCache(0x1775040,0xc0);
    lVar1 = sceSifCallRpc(0x1774b40,5,1,0x1774bc0,0x30,0x1776100,4,0x279e08);
    if (lVar1 == 0) {
      (*(uint32_t*)rtguest::G(0x2e89b8)) = 5;
    }
    else {
      SignalSema((*(uint32_t*)rtguest::G(0x2e89bc)));
    }
  }
  return lVar1;
}


RT_GUEST_FN(0x00279e98, guest::sceMcRead)

}  // namespace guest
