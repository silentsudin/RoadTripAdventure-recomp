// guest:0x0022c3a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t getShopPacketArea(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
  }
  int iVar1;
  
  iVar1 = param_1;
  if (param_1 < 0) {
    iVar1 = param_1 + 0xf;
  }
  rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2ebbe0u)), iVar1 >> 4);
  if (0x1000 < param_1) {
    RT_CALLRA(0x22c3f4, guest::fatal(0x2ebbf8,iVar1 >> 4));
  }
  return 0x1785ff0;
}


RT_GUEST_FN(0x0022c3a8, guest::getShopPacketArea)

}  // namespace guest
