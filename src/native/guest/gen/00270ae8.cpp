// guest:0x00270ae8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceDmaWatch(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x40) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x44) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x30, 8);
    rtguest::setS32(0, (uint32_t)param_4);  // walk: = param_4
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(2, (uint32_t)param_2);  // walk: = param_2
  }
  uint uVar1;
  bool bVar2;
  
  if (param_3 == 1) {
    bVar2 = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(uint32_t))) < param_2;
  }
  else {
    uVar1 = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(uint32_t)));
    if (param_4 == 0) {
      param_4 = 0x1000000;
    }
    while (uVar1 < param_2) {
      param_4 = param_4 + -1;
      if (param_4 < 0) {
        rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x330178u)));
        if (((*(uint32_t*)rtguest::G((uint32_t)param_1)) >> 8 & 1) != 0) {
          (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1)) & 0xfffffeff;
        }
      }
      uVar1 = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(uint32_t)));
    }
    bVar2 = false;
  }
  return bVar2;
}


RT_GUEST_FN(0x00270ae8, guest::sceDmaWatch)

}  // namespace guest
