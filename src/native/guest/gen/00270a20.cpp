// guest:0x00270a20 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceDmaSync(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x20, 8);
    rtguest::setS32(0, (uint32_t)param_3);  // walk: = param_3
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
  }
  uint uVar1;
  
  if (param_2 == 1) {
    uVar1 = (*(uint32_t*)rtguest::G((uint32_t)param_1)) >> 8 & 1;
  }
  else {
    uVar1 = (*(uint32_t*)rtguest::G((uint32_t)param_1));
    if (param_3 == 0) {
      param_3 = 0x1000000;
    }
    while ((uVar1 & 0x100) != 0) {
      param_3 = param_3 + -1;
      if (param_3 < 0) {
        rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x330178u)));
        if (((*(uint32_t*)rtguest::G((uint32_t)param_1)) >> 8 & 1) != 0) {
          (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1)) & 0xfffffeff;
        }
      }
      uVar1 = (*(uint32_t*)rtguest::G((uint32_t)param_1));
    }
    uVar1 = 0;
  }
  return uVar1;
}


RT_GUEST_FN(0x00270a20, guest::sceDmaSync)

}  // namespace guest
