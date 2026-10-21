// guest:0x002243e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SpawnCarSmoke(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
  rtguest::FrameGuard rt_fg(192);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x68, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x78, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x80, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x88, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x90, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x70, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x98) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x9c) = 0x00000000u;
    rtguest::rtSpillF(rt_fg, 6, rt_sv, 0xb0);
    rtguest::rtSpillF(rt_fg, 5, rt_sv, 0xa8);
    rtguest::rtSpillF(rt_fg, 4, rt_sv, 0xa0);
    rtguest::setS32(0, (uint32_t)param_4);  // walk: = param_4
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(3, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(4, (uint32_t)param_3);  // walk: = param_3
    rtguest::setS32(5, (uint32_t)param_5);  // walk: = param_5
  }
uint32_t afStack_c0 = rtguest::allocFrame(12);
    uint32_t uStack_b4 = rtguest::allocFrame(4);
    uint32_t uStack_b0 = rtguest::allocFrame(4);
    uint32_t uStack_ac = rtguest::allocFrame(4);
    uint32_t fStack_a8 = rtguest::allocFrame(4);
    uint32_t uStack_a4 = rtguest::allocFrame(4);
uint32_t auStack_a0 = rtguest::allocFrame(48);
uint32_t auStack_70 = rtguest::allocFrame(12);
    uint32_t fStack_64 = rtguest::allocFrame(4);
uint32_t pfVar1;
  int iVar2;
  int iVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_4));
  if (0x10 < iVar2) {
    (*(int32_t*)rtguest::G((uint32_t)param_4)) = 0;
    iVar2 = 0;
  }
  if (iVar2 < 1) {
    (*(int32_t*)rtguest::G((uint32_t)param_4)) = 0;
    fVar4 = (float)iVar2;
    fVar6 = fVar4 * -0.015625;
    (*(float*)rtguest::G((uint32_t)fStack_a8)) = fVar4 * (*(float*)rtguest::G(0x332d4c)) + (*(float*)rtguest::G(0x332d50));
    fVar4 = fVar4 * -0.03125 + (*(float*)rtguest::G(0x332d54));
  }
  else {
    fVar6 = (float)iVar2 * 0.03125;
    (*(float*)rtguest::G((uint32_t)fStack_a8)) = (*(float*)rtguest::G(0x332d44)) - (float)iVar2 * (*(float*)rtguest::G(0x332d40));
    fVar4 = fVar6 + (*(float*)rtguest::G(0x332d48));
    if (iVar2 == 0x10) {
      (*(int32_t*)rtguest::G((uint32_t)param_4)) = 0;
    }
  }
  fVar5 = 1.0;
  (*(float*)rtguest::G((uint32_t)afStack_c0 + (uint32_t)(0) * 4)) = (*(float*)rtguest::G(0x332d58));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b0)) = (*(uint32_t*)rtguest::G(0x332d5c));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_a4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = 0;
  (*(float*)rtguest::G((uint32_t)afStack_c0 + (uint32_t)(1) * 4)) = 0.0;
  (*(float*)rtguest::G((uint32_t)afStack_c0 + (uint32_t)(2) * 4)) = (*(float*)rtguest::G((uint32_t)fStack_a8));
  RT_CALLRA(0x224508, guest::vu0ApplyMatrix(afStack_c0,param_1,afStack_c0));
  RT_CALLRA(0x22451c, guest::vu0ApplyMatrix(uStack_b0,param_1,uStack_b0));
  if ((*(int32_t*)rtguest::G((uint32_t)param_4)) != 0) {
    RT_CALLRA(0x224544, guest::drawCarEffectB(fVar4,fVar6,param_2 >> 2 & 3,param_3,afStack_c0,2));
  }
  if (fVar5 < SQRT(((*(float*)rtguest::G((uint32_t)afStack_c0 + (uint32_t)(0) * 4)) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff70 + (*(int16_t*)rtguest::G((uint32_t)param_5)) * 0x10)))) *
                   ((*(float*)rtguest::G((uint32_t)afStack_c0 + (uint32_t)(0) * 4)) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff70 + (*(int16_t*)rtguest::G((uint32_t)param_5)) * 0x10)))) +
                   ((*(float*)rtguest::G((uint32_t)afStack_c0 + (uint32_t)(2) * 4)) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff78 + (*(int16_t*)rtguest::G((uint32_t)param_5)) * 0x10)))) *
                   ((*(float*)rtguest::G((uint32_t)afStack_c0 + (uint32_t)(2) * 4)) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff78 + (*(int16_t*)rtguest::G((uint32_t)param_5)) * 0x10)))))) {
    RT_CALLRA(0x2245c0, guest::vu0CopyVector(auStack_70,afStack_c0));
    RT_CALLRA(0x2245d0, guest::vu0MulMatrix(auStack_a0,param_6,auStack_a0));
    if (0.0 < (*(float*)rtguest::G((uint32_t)fStack_64))) {
      pfVar1 = 0x0172ff7c;
      iVar2 = 0;
      if (0.0 < (*(float*)rtguest::G(0x172ff7c))) {
        iVar2 = 2;
        while ((pfVar1 = pfVar1 + 0x20, iVar2 < 0x80 && (0.0 < (*(float*)rtguest::G((uint32_t)pfVar1))))) {
          iVar2 = iVar2 + 2;
        }
      }
      iVar3 = iVar2 * 0x10;
      if (iVar2 != 0x80) {
        (*(int16_t*)rtguest::G((uint32_t)param_5)) = (short)iVar2;
        (*(uint8_t*)rtguest::G((uint32_t)0x1730770 + (uint32_t)(iVar2))) = 2;
        RT_CALLRA(0x22465c, guest::vu0CopyVector(0x0172ff70 + iVar3,afStack_c0));
        RT_CALLRA(0x22466c, guest::vu0CopyVector(iVar3 + 0x172ff80,uStack_b0));
        (*(uint32_t*)rtguest::G((uint32_t)0x172ff7c + (uint32_t)(iVar2 * 4) * 4)) = rtguest::f2bits(0.5f);
        (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff8c + iVar3))) = fVar4;
      }
    }
  }
  return 0;
}


RT_GUEST_FN(0x002243e0, guest::SpawnCarSmoke)

}  // namespace guest
