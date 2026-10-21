// guest:0x0026f738 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceGsSwapDBuffDc(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
  }
  RT_CALLRA(0x26f758, guest::sceGsPutDispEnv((param_2 & 1) * 0x28 + param_1));
  if ((param_2 & 1) == 0) {
    RT_CALLRA(0x26f768, guest::sceGsPutDrawEnv(param_1 + 0x50));
  }
  else {
    RT_CALLRA(0x26f778, guest::sceGsPutDrawEnv(param_1 + 0x1c0));
  }
  return 0;
}


RT_GUEST_FN(0x0026f738, guest::sceGsSwapDBuffDc)

}  // namespace guest
