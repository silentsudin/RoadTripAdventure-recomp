// guest:0x002745a0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t scePadSetVrefParam(uint64_t param_1, uint64_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x20, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x40) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x44) = 0x00000000u;
    rtguest::gS[0] = 0x0000000001773500ull;  // walk const
    rtguest::setS32(1, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(2, (uint32_t)param_1);  // walk: = param_1
  }
  int iVar1;
  long lVar2;
  
    // param_3 is undefined8*: +1 = +8 bytes; *param_3 is an 8-byte load; packet word 0x177350c is
    // stored 8 bytes wide (Ghidra's _DAT_ prefix) per the pseudocode.
  (*(uint32_t*)rtguest::G(0x1773504)) = (uint32_t)param_1;
  (*(uint32_t*)rtguest::G(0x1773500)) = 0xb;
  (*(uint32_t*)rtguest::G(0x1773508)) = (uint32_t)param_2;
  (*(uint64_t*)rtguest::G(0x177350c)) = (*(uint64_t*)rtguest::G((uint32_t)param_3));
  (*(uint32_t*)rtguest::G(0x1773514)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 8)));
  lVar2 = RT_CALLRA(0x274618, guest::sceSifCallRpc(0x17731c0,1,0,0x1773500,0x80,0x1773500,0x80,0));
  if (lVar2 < 0) {
    iVar1 = 0;
  }
  else {
    iVar1 = (*(uint32_t*)rtguest::G(0x177351c));
    if ((*(uint32_t*)rtguest::G(0x177351c)) == 1) {
      RT_CALLRA(0x274644, guest::scePadSetReqState(param_1,param_2,2));
      iVar1 = (*(uint32_t*)rtguest::G(0x177351c));
    }
  }
  return iVar1;
}


RT_GUEST_FN(0x002745a0, guest::scePadSetVrefParam)

}  // namespace guest
