// guest:0x00272568 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gifPkAppendTagNreg1(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x40) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x44) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(2, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(3, (uint32_t)param_3);  // walk: = param_3
  }
uint32_t puVar1;
  int iVar2;
  
  iVar2 = RT_CALLRA(0x27258c, guest::gifPkAlign16());
  puVar1 = (uint32_t)(*(int32_t*)rtguest::G((uint32_t)param_1));
  (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int32_t))) = iVar2;
  (*(uint32_t*)rtguest::G((uint32_t)puVar1)) = param_4 | 0x10000000;
  (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(2) * sizeof(uint32_t))) = param_2;
  (*(int32_t*)rtguest::G((uint32_t)param_1)) = (int)(puVar1 + 4);
  (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(3) * sizeof(uint32_t))) = param_3;
  (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(1) * sizeof(uint32_t))) = 0;
  return 0;
}


RT_GUEST_FN(0x00272568, guest::gifPkAppendTagNreg1)

}  // namespace guest
