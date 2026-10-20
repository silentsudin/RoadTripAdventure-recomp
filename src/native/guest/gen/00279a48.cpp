// guest:0x00279a48 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceMcGetSlotMax(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x40) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x44) = 0x00000000u;
  }
  int iVar1;
  long lVar2;
  
  lVar2 = RT_CALLRA(0x279a64, guest::PollSema((*(uint32_t*)rtguest::G(0x2e89bc))));
  if (lVar2 < 0) {
    iVar1 = -200;
  }
  else if ((*(uint32_t*)rtguest::G(0x1774b64)) == 0) {
    RT_CALLRA(0x279a8c, guest::SignalSema((*(uint32_t*)rtguest::G(0x2e89bc))));
    iVar1 = -100;
  }
  else {
    (*(uint32_t*)rtguest::G(0x1774bc4)) = param_1;
    iVar1 = RT_CALLRA(0x279ac0, guest::sceSifCallRpc(0x1774b40,0x15,0,0x1774bc0,0x30,0x1776100,4,0));
    if (iVar1 == 0) {
      RT_CALLRA(0x279ad4, guest::SignalSema((*(uint32_t*)rtguest::G(0x2e89bc))));
      iVar1 = (*(uint32_t*)rtguest::G(0x1776100));
    }
    else {
      RT_CALLRA(0x279ae4, guest::SignalSema((*(uint32_t*)rtguest::G(0x2e89bc))));
    }
  }
  return iVar1;
}


RT_GUEST_FN(0x00279a48, guest::sceMcGetSlotMax)

}  // namespace guest
