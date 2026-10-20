// guest:0x0027a2e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint64_t sceMcGetInfo(uint32_t param_1, uint32_t param_2, uint64_t param_3, uint64_t param_4, uint64_t param_5, ...) {
  rtguest::FrameGuard rt_fg(160);
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
    (*(uint32_t*)rtguest::G(0x1774bd4)) = (uint32_t)(param_3 != 0);
    (*(uint32_t*)rtguest::G(0x1774bd0)) = (uint32_t)(param_4 != 0);
    (*(uint32_t*)rtguest::G(0x1774bcc)) = (uint32_t)(param_5 != 0);
    (*(uint32_t*)rtguest::G(0x1774b68)) = (uint32_t)param_3;
    (*(uint32_t*)rtguest::G(0x1774b6c)) = (uint32_t)param_4;
    (*(uint32_t*)rtguest::G(0x1774b70)) = (uint32_t)param_5;
    (*(uint32_t*)rtguest::G(0x1774bc4)) = param_1;
    (*(uint32_t*)rtguest::G(0x1774bc8)) = param_2;
    sceSifWriteBackDCache(0x1775040,0xc0);
    lVar1 = sceSifCallRpc(0x1774b40,1,1,0x1774bc0,0x30,0x1776100,4,0x27a288);
    if (lVar1 == 0) {
      (*(uint32_t*)rtguest::G(0x2e89b8)) = 1;
    }
    else {
      SignalSema((*(uint32_t*)rtguest::G(0x2e89bc)));
    }
  }
  return lVar1;
}


RT_GUEST_FN(0x0027a2e0, guest::sceMcGetInfo)

}  // namespace guest
