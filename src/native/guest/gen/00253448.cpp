// guest:0x00253448 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SteerAiToDefaultWaypoint(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
uint32_t afStack_60 = rtguest::allocFrame(24);
    uint32_t fStack_48 = rtguest::allocFrame(4);
  short sVar1;
  int iVar2;
  float fVar3;
  undefined2 uVar4;
uint32_t pfVar5;
  int iVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  
  fVar3 = (*(float*)rtguest::G(0x333048));
  (*(float*)rtguest::G((uint32_t)afStack_60 + (uint32_t)(0) * 4)) = 320.0;
  iVar6 = (int)param_1;
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 600)));
  if (2 < (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24a)))) {
    (*(float*)rtguest::G((uint32_t)afStack_60 + (uint32_t)(0) * 4)) = 80.0;
  }
  (*(float*)rtguest::G((uint32_t)afStack_60 + (uint32_t)(1) * 4)) = 0.0;
  (*(float*)rtguest::G((uint32_t)afStack_60 + (uint32_t)(3) * 4)) = 0.0;
  (*(float*)rtguest::G((uint32_t)afStack_60 + (uint32_t)(2) * 4)) = 200.0;
  fVar9 = 32768.0;
  fVar7 = rtguest::fbits(atan2f(rtguest::f2bits((*(float*)rtguest::G((uint32_t)afStack_60 + (uint32_t)(0) * 4)) - (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x40)))),rtguest::f2bits(200.0f - (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x48))))));
  sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x1d4)));
  fVar7 = (fVar7 / fVar3) * fVar9;
  vu0SubVector(afStack_60,iVar2 + 0x40,afStack_60);
  vecNormalizeVu0(afStack_60,afStack_60);
  vu0ScaleVector((*(float*)rtguest::G(0x33304c)),afStack_60,afStack_60);
  vu0AddVector(afStack_60,afStack_60,iVar2 + 0x40);
  pfVar5 = afStack_60 + 4;
  vu0SubVector(pfVar5,afStack_60,(uint32_t)(iVar6 + 0x90));
  vu0MulVector(pfVar5,pfVar5,pfVar5);
  fVar8 = rtguest::fbits(atan2f(rtguest::f2bits((*(float*)rtguest::G((uint32_t)afStack_60 + (uint32_t)(0) * 4)) - (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x90)))),rtguest::f2bits(
                        (*(float*)rtguest::G((uint32_t)afStack_60 + (uint32_t)(2) * 4)) - (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x98))))));
  uVar4 = (uint32_t)(int)((fVar8 / fVar3) * fVar9 - (float)(int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x1d4))));
  if ((uint32_t)((short)(int)(fVar7 - (float)(int)sVar1) + 0x7ffU) < 0xfff) {
    SetAiSteeringTarget(param_1,uVar4);
  }
  else if (20.0 < SQRT((*(float*)rtguest::G((uint32_t)afStack_60 + (uint32_t)(4) * 4)) + (*(float*)rtguest::G((uint32_t)fStack_48)))) {
    SetAiSteeringTarget(param_1,uVar4,0);
  }
  else {
    SetAiSteeringTarget(param_1,uVar4,0x2d);
  }
  return 0;
}


RT_GUEST_FN(0x00253448, guest::SteerAiToDefaultWaypoint)

}  // namespace guest
