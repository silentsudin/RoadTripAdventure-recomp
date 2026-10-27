// guest:0x002085b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ReflectVectorVU0(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x38, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x28, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x30, 8);
    rtguest::rtSpillF(rt_fg, 4, rt_sv, 0x48);
    *(uint32_t*)rtguest::G(rt_sv+0x40) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x44) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_3);  // walk: = param_3
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
    rtguest::gS[3] = rt_sv + 0x10ull;  // walk: sp+0x10
  }
  float fVar1;
  float fVar2;
uint32_t auStack_50 = rtguest::allocFrame(16);
uint32_t auStack_40 = rtguest::allocFrame(16);
  
  RT_CALLRA(0x2085dc, guest::vecNormalizeVu0(auStack_40, param_2));  // W13c19: fn/002085b0.c:15 drops a1; MIPS 0x2085cc move $4,$19 only, $5 untouched until jal 0x2727e8 @0x2085dc (move $18,$5 in delay slot)
  fVar1 = rtguest::fbits(RT_CALLRA(0x2085e8, guest::vecDotProductVu0(param_3,param_3)));
  fVar1 = SQRT(fVar1);
  if (fVar1 == 0.0) {
    RT_CALLRA(0x208684, guest::vu0CopyVector(param_1,param_3));
  }
  else {
    RT_CALLRA(0x208634, guest::vecDivByScalarVu0(-fVar1,auStack_50,param_3));
    fVar2 = rtguest::fbits(RT_CALLRA(0x208640, guest::vecDotProductVu0(auStack_50,param_2)));
    RT_CALLRA(0x208650, guest::sceVu0ScaleVectorXYZ(fVar2 + fVar2,param_1,auStack_40));
    RT_CALLRA(0x208660, guest::vu0SubVector(param_1,param_1,auStack_50));
    RT_CALLRA(0x208670, guest::vu0ScaleVector(fVar1,param_1,param_1));
  }
  return 0;
}


RT_GUEST_FN(0x002085b0, guest::ReflectVectorVU0)

}  // namespace guest
