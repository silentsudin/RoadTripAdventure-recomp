// guest:0x0027eb68 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _signal_r(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(1, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(2, (uint32_t)param_3);  // walk: = param_3
  }
  undefined4 uVar1;
  int iVar2;
  long lVar3;
uint32_t puVar4;
  
  if (param_2 < 0x20) {
    iVar2 = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0x75) * sizeof(uint32_t)));
    if (iVar2 == 0) {
      lVar3 = RT_CALLRA(0x27ebac, guest::_init_signal_r(param_1));  // 27ebac: $4 intact 27eb8c entry; newlib signal->_init_signal_r(reent)
      if (lVar3 != 0) {
        return 0xffffffff;
      }
      iVar2 = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0x75) * sizeof(uint32_t)));
    }
    puVar4 = (uint32_t)((int)param_2 * 4 + iVar2);
    uVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar4));
    (*(uint32_t*)rtguest::G((uint32_t)puVar4)) = param_3;
  }
  else {
    uVar1 = 0xffffffff;
    (*(uint32_t*)rtguest::G((uint32_t)param_1)) = 0x16;
  }
  return uVar1;
}


RT_GUEST_FN(0x0027eb68, guest::_signal_r)

}  // namespace guest
