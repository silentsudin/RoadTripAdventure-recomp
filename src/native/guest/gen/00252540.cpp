// guest:0x00252540 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t NormalizeQuatAndRotate(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x28, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x38) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x3c) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(2, (uint32_t)param_1);  // walk: = param_1
  }
uint32_t auStack_40 = rtguest::allocFrame(16);
    uint32_t uStack_30 = rtguest::allocFrame(8);
    uint32_t uStack_28 = rtguest::allocFrame(8);
  float fVar1;
  
  RT_CALLRA(0x252568, guest::sceVu0ScaleVectorXYZ(rtguest::fbits(0xbf800000),auStack_40,param_3));
  if (param_2 == 0) {
    (*(uint64_t*)rtguest::G((uint32_t)uStack_30)) = 0x3f80000000000000;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_28)) = 0;
  }
  else {
    (*(uint64_t*)rtguest::G((uint32_t)uStack_30)) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)param_2));
    (*(uint64_t*)rtguest::G((uint32_t)uStack_28)) = (*(uint64_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * 8));
  }
  RT_CALLRA(0x2525bc, guest::vecNormalizeVu0(uStack_30,uStack_30));
  fVar1 = rtguest::fbits(RT_CALLRA(0x2525c8, guest::vecDotProductVu0(uStack_30,auStack_40)));
  RT_CALLRA(0x2525d8, guest::sceVu0ScaleVectorXYZ(fVar1 + fVar1,param_1,uStack_30));
  RT_CALLRA(0x2525e8, guest::vu0SubVector(param_1,param_1,auStack_40));
  return 0;
}


RT_GUEST_FN(0x00252540, guest::NormalizeQuatAndRotate)

}  // namespace guest
