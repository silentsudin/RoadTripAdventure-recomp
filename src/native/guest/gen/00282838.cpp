// guest:0x00282838 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __sfmoreglue(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
    rtguest::setS32(2, (uint32_t)param_2);  // walk: = param_2
  }
  long lVar1;
uint32_t puVar2;
  
  lVar1 = RT_CALLRA(0x282864, guest::_malloc_r(param_1,param_2 * 0x58 + 0xc));
  puVar2 = (uint32_t)lVar1;
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(1) * sizeof(uint32_t))) = param_2;
    (*(uint32_t*)rtguest::G((uint32_t)puVar2)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(2) * sizeof(uint32_t))) = puVar2 + 3;
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(puVar2 + 3)), 0, (long)(param_2 * 0x58)));
  }
  return lVar1;
}


RT_GUEST_FN(0x00282838, guest::__sfmoreglue)

}  // namespace guest
