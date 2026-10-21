// guest:0x00270930 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceDmaRecvI(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x50) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x54) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x20, 8);
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
    rtguest::gS[1] = 0x0000000001000000ull;  // walk const
    rtguest::setS32(3, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(4, (uint32_t)param_3);  // walk: = param_3
  }
  int iVar1;
  
  if (((*(uint32_t*)rtguest::G((uint32_t)param_1)) & 0x100) != 0) {
    iVar1 = 0xffffff;
    do {
      if (iVar1 < 0) {
        rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x330178u)));
        if (((*(uint32_t*)rtguest::G((uint32_t)param_1)) >> 8 & 1) != 0) {
          (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1)) & 0xfffffeff;
        }
      }
      iVar1 = iVar1 + -1;
    } while (((*(uint32_t*)rtguest::G((uint32_t)param_1)) & 0x100) != 0);
  }
  if ((*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(uint32_t))) != 0xffffffff) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(uint32_t))) = param_2;
  }
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(8) * sizeof(uint32_t))) = param_3;
  (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1)) & 0xfffffff2 | 0x108;
  return 0;
}


RT_GUEST_FN(0x00270930, guest::sceDmaRecvI)

}  // namespace guest
