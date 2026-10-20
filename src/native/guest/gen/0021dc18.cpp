// guest:0x0021dc18 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ComputeChaseCameraBasis(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(112);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x48) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4c) = 0x00000000u;
  }
  ushort uVar1;
  bool bVar2;
  int iVar3;
  int iVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
uint32_t afStack_70 = rtguest::allocFrame(16);
uint32_t auStack_60 = rtguest::allocFrame(16);
  
  RT_CALLRA(0x21dc58, guest::vu0CopyVector(afStack_70,param_4 + 0x10));
  fVar8 = (float)(int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x1d4))) * (*(float*)rtguest::G(0x332c2c)) * 3.0517578e-05f;
  fVar5 = rtguest::fbits(RT_CALLRA(0x21dc88, guest::sinf(rtguest::f2bits(fVar8))));
  fVar6 = rtguest::fbits(RT_CALLRA(0x21dc94, guest::cosf(rtguest::f2bits(fVar8))));
  fVar8 = (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(0) * 4)) * fVar5 + (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(2) * 4)) * fVar6;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x1c))) != 0) {
    uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x198)));
    iVar3 = (uVar1 & 1) * 0x10;
    fVar7 = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff30 + iVar3)));
    fVar5 = ((*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(0) * 4)) * fVar6 - (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(2) * 4)) * fVar5) - fVar7;
    if (0.0 < fVar5) {
      fVar6 = (*(float*)rtguest::G(0x332c38));
      if ((*(float*)rtguest::G(0x332c38)) <= (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff34 + iVar3)))) {
        fVar6 = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff34 + iVar3))) + (*(float*)rtguest::G(0x332c38));
      }
      (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff34 + iVar3))) = fVar6;
      fVar6 = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff34 + iVar3)));
      bVar2 = fVar6 < fVar5;
LAB_0021de30:
      if (bVar2) {
        fVar5 = fVar6;
      }
    }
    else if (fVar5 < 0.0) {
      fVar6 = (*(float*)rtguest::G(0x332c3c));
      if ((*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff34 + iVar3))) <= (*(float*)rtguest::G(0x332c3c))) {
        fVar6 = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff34 + iVar3))) + (*(float*)rtguest::G(0x332c3c));
      }
      (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff34 + iVar3))) = fVar6;
      fVar6 = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff34 + iVar3)));
      bVar2 = fVar5 < fVar6;
      goto LAB_0021de30;
    }
    (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(0) * 4)) = fVar7 + fVar5;
    (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff34 + iVar3))) = fVar5;
    iVar4 = (uVar1 & 1) * 0x10;
    (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff30 + iVar3))) = (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(0) * 4));
    (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(2) * 4)) = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff38 + iVar4)));
    fVar8 = fVar8 - (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(2) * 4));
    (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(0) * 4)) = (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(0) * 4)) - (*(float*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x50)));
    if (0.0 < fVar8) {
      fVar5 = (*(float*)rtguest::G(0x332c40));
      if ((*(float*)rtguest::G(0x332c40)) <= (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff3c + iVar4)))) {
        fVar5 = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff3c + iVar4))) + (*(float*)rtguest::G(0x332c40));
      }
      (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff3c + iVar4))) = fVar5;
      fVar5 = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff3c + iVar4)));
      bVar2 = fVar5 < fVar8;
LAB_0021dee8:
      if (bVar2) {
        fVar8 = fVar5;
      }
    }
    else if (fVar8 < 0.0) {
      fVar5 = (*(float*)rtguest::G(0x332c44));
      if ((*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff3c + iVar4))) <= (*(float*)rtguest::G(0x332c44))) {
        fVar5 = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff3c + iVar4))) + (*(float*)rtguest::G(0x332c44));
      }
      (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff3c + iVar4))) = fVar5;
      fVar5 = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff3c + iVar4)));
      bVar2 = fVar8 < fVar5;
      goto LAB_0021dee8;
    }
    (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(2) * 4)) = (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(2) * 4)) + fVar8;
    (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff3c + iVar4))) = fVar8;
    (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff38 + iVar4))) = (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(2) * 4));
    (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(2) * 4)) = (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(2) * 4)) + (*(float*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x58)));
    goto LAB_0021df08;
  }
  iVar3 = ((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x198))) & 1) * 0x10;
  (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(0) * 4)) = -(*(float*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x50)));
  (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(2) * 4)) = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff38 + iVar3)));
  fVar8 = fVar8 - (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(2) * 4));
  if (0.0 < fVar8) {
    fVar5 = (*(float*)rtguest::G(0x332c30));
    if ((*(float*)rtguest::G(0x332c30)) <= (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff3c + iVar3)))) {
      fVar5 = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff3c + iVar3))) + (*(float*)rtguest::G(0x332c30));
    }
    (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff3c + iVar3))) = fVar5;
    fVar5 = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff3c + iVar3)));
    bVar2 = fVar5 < fVar8;
LAB_0021dd70:
    if (bVar2) {
      fVar8 = fVar5;
    }
  }
  else if (fVar8 < 0.0) {
    fVar5 = (*(float*)rtguest::G(0x332c34));
    if ((*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff3c + iVar3))) <= (*(float*)rtguest::G(0x332c34))) {
      fVar5 = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff3c + iVar3))) + (*(float*)rtguest::G(0x332c34));
    }
    (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff3c + iVar3))) = fVar5;
    fVar5 = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff3c + iVar3)));
    bVar2 = fVar8 < fVar5;
    goto LAB_0021dd70;
  }
  (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(2) * 4)) = (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(2) * 4)) + fVar8;
  (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff3c + iVar3))) = fVar8;
  (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(1) * 4)) = 1.0f;
  (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff38 + iVar3))) = (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(2) * 4));
LAB_0021df08:
  fVar5 = (float)((int)(((uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x16))) + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x1a)))) *
                       0x10000) >> 0x10) * (*(float*)rtguest::G(0x332c48)) * 3.0517578e-05f;
  fVar8 = (*(float*)rtguest::G(0x332c48));
  fVar6 = rtguest::fbits(RT_CALLRA(0x21df3c, guest::sinf(rtguest::f2bits(fVar5))));
  fVar7 = rtguest::fbits(RT_CALLRA(0x21df48, guest::cosf(rtguest::f2bits(fVar5))));
  fVar5 = (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(2) * 4)) * fVar6;
  (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(2) * 4)) = (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(0) * 4)) * fVar6 + (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(2) * 4)) * fVar7;
  (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(0) * 4)) = (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(0) * 4)) * fVar7 - fVar5;
  RT_CALLRA(0x21df80, guest::vecNormalizeVu0(auStack_60,afStack_70));
  fVar5 = (float)((int)(((uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x1a))) +
                        (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x1d4))) + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x16)))) *
                       -0x10000) >> 0x10) * fVar8 * 3.0517578e-05f;
  fVar6 = rtguest::fbits(RT_CALLRA(0x21dfbc, guest::sinf(rtguest::f2bits(fVar5))));
  fVar7 = rtguest::fbits(RT_CALLRA(0x21dfc8, guest::cosf(rtguest::f2bits(fVar5))));
  fVar5 = (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(2) * 4)) * fVar6;
  (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(2) * 4)) = (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(0) * 4)) * fVar6 + (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(2) * 4)) * fVar7;
  (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(0) * 4)) = (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(0) * 4)) * fVar7 - fVar5;
  RT_CALLRA(0x21dffc, guest::vecNormalizeVu0(afStack_70,afStack_70));
  RT_CALLRA(0x21e008, guest::BuildYawTransform(param_2,auStack_60));
  RT_CALLRA(0x21e014, guest::BuildYawTransform(param_1,afStack_70));
  RT_CALLRA(0x21e034, guest::ApplyMatrixRotation(rtguest::f2bits((float)(int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x1d4))) * fVar8 * 3.0517578e-05f),param_1));
  return 0;
}


RT_GUEST_FN(0x0021dc18, guest::ComputeChaseCameraBasis)

}  // namespace guest
