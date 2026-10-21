// guest:0x00224068 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SpawnCarDustPuffs(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, ...) {
  rtguest::FrameGuard rt_fg(112);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x50) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x54) = 0x00000000u;
  }
uint32_t afStack_70 = rtguest::allocFrame(12);
    uint32_t uStack_64 = rtguest::allocFrame(4);
    uint32_t uStack_60 = rtguest::allocFrame(4);
    uint32_t uStack_5c = rtguest::allocFrame(4);
    uint32_t fStack_58 = rtguest::allocFrame(4);
    uint32_t uStack_54 = rtguest::allocFrame(4);
  undefined4 uVar1;
uint32_t pfVar2;
  int iVar3;
  int iVar4;
  float fVar5;
  float fVar6;
  
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_4));
  if (0x20 < iVar3) {
    (*(int32_t*)rtguest::G((uint32_t)param_4)) = 0;
    iVar3 = 0;
  }
  if (0x10 < iVar3) {
    (*(uint32_t*)rtguest::G((uint32_t)uStack_64)) = 0x3f800000;
    (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(0) * 4)) = (*(float*)rtguest::G(0x332d08));
    (*(uint32_t*)rtguest::G((uint32_t)uStack_60)) = (*(uint32_t*)rtguest::G(0x332d0c));
    (*(uint32_t*)rtguest::G((uint32_t)uStack_54)) = 0x3f800000;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) = 0;
    (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(1) * 4)) = 0.0;
    RT_CALLRA(0x2240f4, guest::vu0ApplyMatrix(afStack_70,param_1,afStack_70));
    RT_CALLRA(0x224108, guest::vu0ApplyMatrix(uStack_60,param_1,uStack_60));
    fVar5 = (float)((*(int32_t*)rtguest::G((uint32_t)param_4)) + -0x10);
    (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(2) * 4)) = (*(float*)rtguest::G(0x332d18)) - fVar5 * (*(float*)rtguest::G(0x332d10));
    (*(float*)rtguest::G((uint32_t)fStack_58)) = (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(2) * 4));
    RT_CALLRA(0x22416c, guest::drawCarEffectB(fVar5 * 0.03125 + (*(float*)rtguest::G(0x332d14)),fVar5 * 0.015625,param_2 >> 2 & 3,param_3,
                   afStack_70,1));
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_4));
  }
  if (iVar3 < 1) {
    (*(int32_t*)rtguest::G((uint32_t)param_4)) = 0;
    fVar5 = (float)iVar3;
    fVar6 = fVar5 * -0.015625;
    (*(float*)rtguest::G((uint32_t)fStack_58)) = fVar5 * (*(float*)rtguest::G(0x332d28)) + (*(float*)rtguest::G(0x332d2c));
    fVar5 = fVar5 * -0.03125 + (*(float*)rtguest::G(0x332d30));
  }
  else {
    fVar5 = (float)iVar3;
    fVar6 = fVar5 * 0.015625;
    (*(float*)rtguest::G((uint32_t)fStack_58)) = (*(float*)rtguest::G(0x332d20)) - fVar5 * (*(float*)rtguest::G(0x332d1c));
    fVar5 = fVar5 * 0.03125 + (*(float*)rtguest::G(0x332d24));
    if (iVar3 == 0x20) {
      (*(int32_t*)rtguest::G((uint32_t)param_4)) = 0x10;
    }
  }
  (*(uint32_t*)rtguest::G((uint32_t)uStack_64)) = 0x3f800000;
  (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(0) * 4)) = (*(float*)rtguest::G(0x332d34));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_60)) = (*(uint32_t*)rtguest::G(0x332d38));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_54)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) = 0;
  (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(1) * 4)) = 0.0;
  (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(2) * 4)) = (*(float*)rtguest::G((uint32_t)fStack_58));
  RT_CALLRA(0x224250, guest::vu0ApplyMatrix(afStack_70,param_1,afStack_70));
  RT_CALLRA(0x224260, guest::vu0ApplyMatrix(uStack_60,param_1,uStack_60));
  if ((*(int32_t*)rtguest::G((uint32_t)param_4)) != 0) {
    RT_CALLRA(0x224288, guest::drawCarEffectB(fVar5,fVar6,param_2 >> 2 & 3,param_3,afStack_70,1));
  }
  if (2.0 < SQRT(((*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(0) * 4)) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff70 + (*(int16_t*)rtguest::G((uint32_t)param_5)) * 0x10)))) *
                 ((*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(0) * 4)) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff70 + (*(int16_t*)rtguest::G((uint32_t)param_5)) * 0x10)))) +
                 ((*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(2) * 4)) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff78 + (*(int16_t*)rtguest::G((uint32_t)param_5)) * 0x10)))) *
                 ((*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(2) * 4)) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff78 + (*(int16_t*)rtguest::G((uint32_t)param_5)) * 0x10)))))) {
    pfVar2 = 0x0172ff7c;
    iVar3 = 0;
    if (0.0 < (*(float*)rtguest::G(0x172ff7c))) {
      iVar3 = 2;
      while ((pfVar2 = pfVar2 + 0x20, iVar3 < 0x80 && (0.0 < (*(float*)rtguest::G((uint32_t)pfVar2))))) {
        iVar3 = iVar3 + 2;
      }
    }
    iVar4 = iVar3 * 0x10;
    if (iVar3 != 0x80) {
      (*(int16_t*)rtguest::G((uint32_t)param_5)) = (short)iVar3;
      (*(uint8_t*)rtguest::G((uint32_t)0x1730770 + (uint32_t)(iVar3))) = 1;
      RT_CALLRA(0x22437c, guest::vu0CopyVector(0x0172ff70 + iVar4,afStack_70));
      RT_CALLRA(0x22438c, guest::vu0CopyVector(iVar4 + 0x172ff80,uStack_60));
      uVar1 = (*(uint32_t*)rtguest::G(0x332d3c));
      (*(uint32_t*)rtguest::G((uint32_t)0x172ff7c + (uint32_t)(iVar3 * 4) * 4)) = rtguest::f2bits(0.25f);
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0172ff8c + iVar4))) = uVar1;
    }
  }
  return 0;
}


RT_GUEST_FN(0x00224068, guest::SpawnCarDustPuffs)

}  // namespace guest
