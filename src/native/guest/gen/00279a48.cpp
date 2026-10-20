// guest:0x00279a48 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceMcGetSlotMax(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  long lVar2;
  
  lVar2 = PollSema((*(uint32_t*)rtguest::G(0x2e89bc)));
  if (lVar2 < 0) {
    iVar1 = -200;
  }
  else if ((*(uint32_t*)rtguest::G(0x1774b64)) == 0) {
    SignalSema((*(uint32_t*)rtguest::G(0x2e89bc)));
    iVar1 = -100;
  }
  else {
    (*(uint32_t*)rtguest::G(0x1774bc4)) = param_1;
    iVar1 = sceSifCallRpc(0x1774b40,0x15,0,0x1774bc0,0x30,0x1776100,4,0);
    if (iVar1 == 0) {
      SignalSema((*(uint32_t*)rtguest::G(0x2e89bc)));
      iVar1 = (*(uint32_t*)rtguest::G(0x1776100));
    }
    else {
      SignalSema((*(uint32_t*)rtguest::G(0x2e89bc)));
    }
  }
  return iVar1;
}


RT_GUEST_FN(0x00279a48, guest::sceMcGetSlotMax)

}  // namespace guest
