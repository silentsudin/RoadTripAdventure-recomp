// guest:0x0024fc30 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t LoadAndTransformVec4(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
  }
  short sVar1;
  int iVar2;
uint32_t piVar3;
uint32_t aiStack_20 = rtguest::allocFrame(16);
  
  piVar3 = aiStack_20;
  iVar2 = 3;
  do {
    sVar1 = (*(int16_t*)rtguest::G((uint32_t)param_2));
    param_2 = param_2 + 1;
    iVar2 = iVar2 + -1;
    (*(int32_t*)rtguest::G((uint32_t)piVar3)) = (int)sVar1;
    piVar3 = piVar3 + 0x4;  // T* element step
  } while (-1 < iVar2);
  RT_CALLRA(0x24fc68, guest::vu0Itof4Vector(param_1,aiStack_20));
  RT_CALLRA(0x24fc78, guest::vu0ScaleVector((*(float*)rtguest::G(0x332f94)),param_1,param_1));
  return param_1;
}


RT_GUEST_FN(0x0024fc30, guest::LoadAndTransformVec4)

}  // namespace guest
