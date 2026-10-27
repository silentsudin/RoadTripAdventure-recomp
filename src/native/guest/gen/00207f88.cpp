// guest:0x00207f88 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t BuildTransformMatrix(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(1, (uint32_t)param_3);  // walk: = param_3
    rtguest::setS32(3, (uint32_t)param_2);  // walk: = param_2
  }
  int iVar1;
  
  iVar1 = (int)param_1 + 0x10;
  RT_CALLRA(0x207fac, guest::vu0UnitMatrix(param_1));  // W13c19: fn/00207f88.c:11 shows vu0UnitMatrix() (Ghidra dropped a0); MIPS 0x207f90 move $16,$4 / jal 0x272a48 @0x207fac leaves a0=param_1
  vecCrossProductVu0(param_1,param_2,param_3);
  RT_CALLRA(0x207fc8, guest::vecNormalizeVu0(param_1,param_1));
  RT_CALLRA(0x207fd4, guest::vu0CopyVector(iVar1,param_2));
  vecCrossProductVu0((int)param_1 + 0x20,param_1,iVar1);
  return 0;
}


RT_GUEST_FN(0x00207f88, guest::BuildTransformMatrix)

}  // namespace guest
