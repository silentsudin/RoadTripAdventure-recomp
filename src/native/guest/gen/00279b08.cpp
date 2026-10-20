// guest:0x00279b08 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int64_t sceMcOpen(uint32_t param_1, uint32_t param_2, int64_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(144);
  long lVar1;
  
  lVar1 = PollSema((*(uint32_t*)rtguest::G(0x2e89bc)));
  if (lVar1 < 0) {
    lVar1 = -200;
  }
  else if ((*(uint32_t*)rtguest::G(0x1774b64)) == 0) {
    SignalSema((*(uint32_t*)rtguest::G(0x2e89bc)));
    lVar1 = -100;
  }
  else if ((param_3 == 0) || ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)param_3)) == '\0')) {
    SignalSema((*(uint32_t*)rtguest::G(0x2e89bc)));
    lVar1 = -0xd2;
  }
  else {
    (uint32_t)(uintptr_t)(::strncpy((char *)rtguest::G((uint32_t)(0x01774c04)), (char *)rtguest::G((uint32_t)((uint32_t)param_3)), 0x3ff));
    (*(uint8_t*)rtguest::G(0x1775003)) = 0;
    (*(uint32_t*)rtguest::G(0x1774bf0)) = param_1;
    (*(uint32_t*)rtguest::G(0x1774bf4)) = param_2;
    (*(uint32_t*)rtguest::G(0x1774bf8)) = param_4;
    lVar1 = sceSifCallRpc(0x1774b40,2,1,0x1774bf0,0x414,0x1776100,4,0);
    if (lVar1 == 0) {
      (*(uint32_t*)rtguest::G(0x2e89b8)) = 2;
    }
    else {
      SignalSema((*(uint32_t*)rtguest::G(0x2e89bc)));
    }
  }
  return lVar1;
}


RT_GUEST_FN(0x00279b08, guest::sceMcOpen)

}  // namespace guest
