// guest:0x00283b00 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _i2b(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_2);  // walk: = param_2
  }
  int iVar1;
  
  iVar1 = RT_CALLRA(0x283b10, guest::_Balloc(param_1,1));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x14))) = param_2;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x10))) = 1;
  return 0;
}


RT_GUEST_FN(0x00283b00, guest::_i2b)

}  // namespace guest
