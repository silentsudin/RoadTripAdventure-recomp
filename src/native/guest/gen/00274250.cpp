// guest:0x00274250 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t scePadSetActAlign(uint64_t param_1, uint64_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x40) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x44) = 0x00000000u;
    rtguest::gS[0] = 0x0000000001773500ull;  // walk const
    rtguest::setS32(1, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(2, (uint32_t)param_1);  // walk: = param_1
  }
uint32_t puVar1;
  long lVar2;
uint32_t puVar3;
  int iVar4;
  
  (*(uint32_t*)rtguest::G(0x1773500)) = 8;
  (*(uint32_t*)rtguest::G(0x1773504)) = (uint32_t)param_1;
  iVar4 = 0;
  (*(uint32_t*)rtguest::G(0x1773508)) = (uint32_t)param_2;
  do {
    puVar1 = (uint32_t)(param_3 + iVar4);
    puVar3 = (uint32_t)((int)0x0177350c + iVar4);
    iVar4 = iVar4 + 1;
    (*(uint8_t*)rtguest::G((uint32_t)puVar3)) = (*(uint8_t*)rtguest::G((uint32_t)puVar1));
  } while (iVar4 < 6);
  lVar2 = RT_CALLRA(0x2742d4, guest::sceSifCallRpc(0x17731c0,1,0,0x1773500,0x80,0x1773500,0x80,0));
  if (lVar2 < 0) {
    iVar4 = 0;
  }
  else {
    iVar4 = (*(uint32_t*)rtguest::G(0x1773514));
    if ((*(uint32_t*)rtguest::G(0x1773514)) == 1) {
      RT_CALLRA(0x274300, guest::scePadSetReqState(param_1,param_2,2));
      iVar4 = (*(uint32_t*)rtguest::G(0x1773514));
    }
  }
  return iVar4;
}


RT_GUEST_FN(0x00274250, guest::scePadSetActAlign)

}  // namespace guest
