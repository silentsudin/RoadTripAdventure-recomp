// guest:0x0024fed8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ComputeWaypointTargetAngle(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(48);
    rtguest::allocFrame(2);  // guest frame gap (sp+0x0..0x1)
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
    uint32_t uStack_2e = rtguest::allocFrame(2);  // sp+0x2 (Ghidra entry-sp -0x2e, frame 0x30)
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  
  fVar4 = (*(uint32_t*)rtguest::G(0x332fa0));
  if (param_3 == 1) {
    fVar3 = ((*(float*)rtguest::G((uint32_t)param_1)) + (*(float*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(float)))) * 0.5 - (*(float*)rtguest::G((uint32_t)param_2));
    fVar2 = ((*(float*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(float))) + (*(float*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(float)))) * 0.5 - (*(float*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(float)));
    fVar4 = (*(float*)rtguest::G(0x332f9c));
  }
  else {
    if (param_3 == 2) {
      fVar2 = rtguest::fbits(RT_CALLRA(0x24ff60, guest::atan2f(rtguest::f2bits((*(float*)rtguest::G((uint32_t)param_1)) - (*(float*)rtguest::G((uint32_t)param_2))),rtguest::f2bits((*(float*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(float))) - (*(float*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(float)))))));
      (*(uint16_t*)rtguest::G((uint32_t)uStack_2e)) = (uint32_t)(int)(fVar2 * fVar4);
      fVar3 = ((*(float*)rtguest::G((uint32_t)param_1)) + (*(float*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(float)))) * 0.5 - (*(float*)rtguest::G((uint32_t)param_2));
      fVar2 = ((*(float*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(float))) + (*(float*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(float)))) * 0.5 - (*(float*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(float)));
      goto LAB_00250004;
    }
    fVar3 = (*(float*)rtguest::G((uint32_t)param_1)) - (*(float*)rtguest::G((uint32_t)param_2));
    fVar2 = (*(float*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(float))) - (*(float*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(float)));
    fVar4 = (*(uint32_t*)rtguest::G(0x332fa4));
  }
  fVar1 = rtguest::fbits(RT_CALLRA(0x24ffd0, guest::atan2f(rtguest::f2bits(fVar3),rtguest::f2bits(fVar2))));
  fVar2 = (*(float*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(float))) - (*(float*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(float)));
  fVar3 = (*(float*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(float))) - (*(float*)rtguest::G((uint32_t)param_2));
  (*(uint16_t*)rtguest::G((uint32_t)uStack_2e)) = (uint32_t)(int)(fVar1 * fVar4);
LAB_00250004:
  fVar2 = rtguest::fbits(RT_CALLRA(0x250004, guest::atan2f(rtguest::f2bits(fVar3),rtguest::f2bits(fVar2))));
  return (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)uStack_2e)) << 0x10 |
         (long)(int)(fVar2 * fVar4) & 0xffffffff00000000U | (uint32_t)(uint32_t)(int)(fVar2 * fVar4);
}


RT_GUEST_FN(0x0024fed8, guest::ComputeWaypointTargetAngle)

}  // namespace guest
