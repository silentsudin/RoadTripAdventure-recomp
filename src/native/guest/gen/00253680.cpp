// guest:0x00253680 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateAiCourseSteering(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(112);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x48, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x58, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x68) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x6c) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
  }
uint32_t afStack_70 = rtguest::allocFrame(8);
    uint32_t fStack_68 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
uint32_t afStack_60 = rtguest::allocFrame(8);
    uint32_t fStack_58 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
uint32_t afStack_50 = rtguest::allocFrame(8);
    uint32_t fStack_48 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
uint32_t afStack_40 = rtguest::allocFrame(8);
    uint32_t fStack_38 = rtguest::allocFrame(4);
  byte bVar1;
  bool bVar2;
  undefined2 uVar3;
  int iVar4;
  long lVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  int iVar9;
uint32_t piVar10;
  float fVar11;
  
  iVar9 = (int)param_1;
  bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24a)));
  uVar8 = (uint32_t)bVar1;
  iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 600)));
  piVar10 = (uint32_t)(iVar6 + (uint32_t)bVar1 * 0xc + 0x70);
  iVar4 = (*(int32_t*)rtguest::G((uint32_t)piVar10));
  uVar7 = (uint32_t)bVar1;
  if (iVar4 == 1) {
    if (((*(float*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x40))) <= (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002ac4b0 + uVar7 * 0x40)))) ||
       (bVar2 = true, (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002ac4b4 + uVar7 * 0x40))) <= (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x40))))) {
      RT_CALLRA(0x25373c, guest::vu0SubVector(afStack_60,iVar6 + 0x40,iVar9 + 0x90));
      RT_CALLRA(0x25374c, guest::vu0MulVector(afStack_60,afStack_60,afStack_60));
      uVar8 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24a)));
      bVar2 = true;
      if ((*(float*)rtguest::G((uint32_t)(uint32_t)(0x002ac4b8 + (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24a))) * 0x40))) <=
          SQRT((*(float*)rtguest::G((uint32_t)afStack_60 + (uint32_t)(0) * 4)) + (*(float*)rtguest::G((uint32_t)fStack_58)))) {
        bVar2 = false;
      }
    }
    if (bVar2) {
      iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 600)));
      iVar4 = uVar8 * 3;
      goto LAB_00253b4c;
    }
    fVar11 = rtguest::fbits(RT_CALLRA(0x2538cc, guest::atan2f(rtguest::f2bits((*(float*)rtguest::G((uint32_t)(uint32_t)(0x002ac4a0 + uVar8 * 0x40))) - (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x90)))),rtguest::f2bits(
                           (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002ac4a8 + uVar8 * 0x40))) - (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x98)))))));
    uVar3 = (uint32_t)
            (int)((rtguest::fdiv(fVar11, (*(float*)rtguest::G(0x333050)))) * 32768.0 - (float)(int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x1d4))));
    RT_CALLRA(0x253818, guest::vu0SubVector(afStack_50,0x002ac4a0 + uVar8 * 0x40,iVar9 + 0x90));
    RT_CALLRA(0x253828, guest::vu0MulVector(afStack_50,afStack_50,afStack_50));
    if (SQRT((*(float*)rtguest::G((uint32_t)afStack_50 + (uint32_t)(0) * 4)) + (*(float*)rtguest::G((uint32_t)fStack_48))) < 20.0) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 600))) + (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24a))) * 0xc + 0x78))) = 0;
      lVar5 = RT_CALLRA(0x2539a8, guest::SetAiSteeringTarget(param_1,uVar3,10));
    }
    else {
      RT_CALLRA(0x2539bc, guest::SetAiSteeringTarget(param_1,uVar3,0));
      lVar5 = 0;
    }
    iVar6 = 2;
    if (lVar5 == 0) {
      return 0;
    }
    bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24a)));
  }
  else {
    if (1 < iVar4) {
      if (iVar4 != 2) {
        return 0;
      }
      if (((*(float*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x40))) <= (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002ac4b0 + uVar7 * 0x40)))) ||
         (bVar2 = true, (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002ac4b4 + uVar7 * 0x40))) <= (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x40))))) {
        RT_CALLRA(0x253920, guest::vu0SubVector(afStack_40,iVar6 + 0x40,iVar9 + 0x90));
        RT_CALLRA(0x253930, guest::vu0MulVector(afStack_40,afStack_40,afStack_40));
        uVar8 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24a)));
        bVar2 = true;
        if ((*(float*)rtguest::G((uint32_t)(uint32_t)(0x002ac4b8 + (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24a))) * 0x40))) <=
            SQRT((*(float*)rtguest::G((uint32_t)afStack_40 + (uint32_t)(0) * 4)) + (*(float*)rtguest::G((uint32_t)fStack_38)))) {
          bVar2 = false;
        }
      }
      if (bVar2) {
        iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 600)));
        iVar4 = uVar8 * 3;
      }
      else {
        iVar6 = (int)(((int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(0x002ac480 + uVar8 * 0x40))) -
                      (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x1d4)))) * 0x10000) >> 0x10;
        if ((iVar6 + 0x3ffU & 0xffff) < 0x7ff) {
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 600))) + uVar8 * 0xc + 0x78))) = 0;
          lVar5 = RT_CALLRA(0x253b14, guest::SetAiSteeringTarget(param_1,iVar6,10));
        }
        else {
          RT_CALLRA(0x253b28, guest::SetAiSteeringTarget(param_1,iVar6,10));
          lVar5 = 0;
        }
        if (lVar5 == 0) {
          return 0;
        }
        iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 600)));
        iVar4 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24a))) * 3;
      }
LAB_00253b4c:
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + iVar4 * 4 + 0x78))) = 0;
      (*(int32_t*)rtguest::G((uint32_t)piVar10)) = 0;
      return 0;
    }
    if (iVar4 != 0) {
      return 0;
    }
    if (((*(float*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x40))) <= (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002ac4b0 + uVar7 * 0x40)))) ||
       (bVar2 = true, (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002ac4b4 + uVar7 * 0x40))) <= (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x40))))) {
      RT_CALLRA(0x253a3c, guest::vu0SubVector(afStack_70,iVar6 + 0x40,iVar9 + 0x90));
      RT_CALLRA(0x253a4c, guest::vu0MulVector(afStack_70,afStack_70,afStack_70));
      bVar2 = true;
      if ((*(float*)rtguest::G((uint32_t)(uint32_t)(0x002ac4b8 + (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24a))) * 0x40))) <=
          SQRT((*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(0) * 4)) + (*(float*)rtguest::G((uint32_t)fStack_68)))) {
        bVar2 = false;
      }
    }
    if (bVar2) {
      RT_CALLRA(0x2537b8, guest::SteerAiToDefaultWaypoint(param_1));
      return 0;
    }
    bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24a)));
    iVar6 = 1;
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 600))) + (uint32_t)bVar1 * 0xc + 0x78))) = 0;
  (*(int32_t*)rtguest::G((uint32_t)piVar10)) = iVar6;
  return 0;
}


RT_GUEST_FN(0x00253680, guest::UpdateAiCourseSteering)

}  // namespace guest
