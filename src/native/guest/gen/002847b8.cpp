// guest:0x002847b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __swrite(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(1, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(2, (uint32_t)param_3);  // walk: = param_3
  }
  undefined4 uVar1;
  
  if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xc))) & 0x100) != 0) {
    RT_CALLRA(0x2847f0, guest::_lseek_r((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x54))),(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xe))),0,2));
  }
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xc))) = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xc))) & 0xefff;
  uVar1 = RT_CALLRA(0x284810, guest::_write_r((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x54))),(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xe))),param_2,param_3));
  return uVar1;
}


RT_GUEST_FN(0x002847b8, guest::__swrite)

}  // namespace guest
