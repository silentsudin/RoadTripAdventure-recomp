// guest:0x00279fb8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint64_t sceMcWrite(uint32_t param_1, uint32_t param_2, uint64_t param_3, ...) {
  rtguest::FrameGuard rt_fg(128);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
    rtguest::setS32(1, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(3, (uint32_t)param_1);  // walk: = param_1
    rtguest::gS[4] = 0x00000000002f0000ull;  // walk const
  }
uint32_t puVar1;
  long lVar2;
  uint uVar3;
  uint uVar4;
  
  lVar2 = RT_CALLRA(0x279fe8, guest::PollSema((*(uint32_t*)rtguest::G(0x2e89bc))));
  if (lVar2 < 0) {
    lVar2 = -200;
  }
  else if ((*(uint32_t*)rtguest::G(0x1774b64)) == 0) {
    RT_CALLRA(0x27a010, guest::SignalSema((*(uint32_t*)rtguest::G(0x2e89bc))));
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
    RT_CALLRA(0x27a0bc, guest::FlushCache(0));
    lVar2 = RT_CALLRA(0x27a0e4, guest::sceSifCallRpc(0x1774b40,6,1,0x1774bc0,0x30,0x1776100,4,0));
    if (lVar2 == 0) {
      (*(uint32_t*)rtguest::G(0x2e89b8)) = 6;
    }
    else {
      RT_CALLRA(0x27a104, guest::SignalSema((*(uint32_t*)rtguest::G(0x2e89bc))));
    }
  }
  return lVar2;
}


RT_GUEST_FN(0x00279fb8, guest::sceMcWrite)

}  // namespace guest
