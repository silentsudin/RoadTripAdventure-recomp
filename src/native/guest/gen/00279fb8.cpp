// guest:0x00279fb8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint64_t sceMcWrite(uint32_t param_1, uint32_t param_2, uint64_t param_3, ...) {
  rtguest::FrameGuard rt_fg(128);
uint32_t puVar1;
  long lVar2;
  uint uVar3;
  uint uVar4;
  
  lVar2 = PollSema((*(uint32_t*)rtguest::G(0x2e89bc)));
  if (lVar2 < 0) {
    lVar2 = -200;
  }
  else if ((*(uint32_t*)rtguest::G(0x1774b64)) == 0) {
    SignalSema((*(uint32_t*)rtguest::G(0x2e89bc)));
    lVar2 = -100;
  }
  else {
    if (param_3 < 0x11) {
      (*(uint32_t*)rtguest::G(0x1774bd8)) = (uint32_t)0x0;
      (*(uint32_t*)rtguest::G(0x1774bcc)) = 0;
      (*(uint32_t*)rtguest::G(0x1774bd4)) = (uint32_t)param_3;
    }
    else {
      (*(uint32_t*)rtguest::G(0x1774bd4)) = ((uint32_t)(param_2 + -1) & 0xfffffff0) - (int)(param_2 + -0x10);
      (*(uint32_t*)rtguest::G(0x1774bcc)) = (uint32_t)param_3 - (*(uint32_t*)rtguest::G(0x1774bd4));
      (*(uint32_t*)rtguest::G(0x1774bd8)) = param_2 + (*(uint32_t*)rtguest::G(0x1774bd4));
    }
    uVar3 = 0;
    puVar1 = param_2;
    (*(uint32_t*)rtguest::G(0x1774bc0)) = param_1;
    if ((*(uint32_t*)rtguest::G(0x1774bd4)) != 0) {
      do {
        uVar4 = uVar3 + 1;
        (*(uint8_t*)rtguest::G((uint32_t)0x1774be0 + (uint32_t)(uVar3) * sizeof(uint8_t))) = (*(uint8_t*)rtguest::G((uint32_t)puVar1));
        puVar1 = param_2 + uVar4;
        uVar3 = uVar4;
      } while (uVar4 < (*(uint32_t*)rtguest::G(0x1774bd4)));
    }
    FlushCache(0);
    lVar2 = sceSifCallRpc(0x1774b40,6,1,0x1774bc0,0x30,0x1776100,4,0);
    if (lVar2 == 0) {
      (*(uint32_t*)rtguest::G(0x2e89b8)) = 6;
    }
    else {
      SignalSema((*(uint32_t*)rtguest::G(0x2e89bc)));
    }
  }
  return lVar2;
}


RT_GUEST_FN(0x00279fb8, guest::sceMcWrite)

}  // namespace guest
