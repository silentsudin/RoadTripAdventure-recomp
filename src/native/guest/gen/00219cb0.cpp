// guest:0x00219cb0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t TestQuadLateralMask(uint32_t rt_bits_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, ...) {
  float param_1 = rtguest::fbits(rt_bits_1);  // f12.. float arg carried as bits (W13 float-param fix)
  rtguest::FrameGuard rt_fg(112);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x38) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x3c) = 0x00000000u;
  }
    uint32_t fStack_70 = rtguest::allocFrame(4);
    uint32_t fStack_6c = rtguest::allocFrame(4);
    uint32_t fStack_68 = rtguest::allocFrame(4);
    uint32_t uStack_64 = rtguest::allocFrame(4);
uint32_t pfVar1;
uint32_t pfVar2;
uint32_t pfVar3;
  ulong uVar4;
  float fVar5;
  float fVar6;
  
  uVar4 = 0;
  if (param_2 != (uint32_t)0x0) {
    param_5 = param_5 + -1;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_64)) = 0;
    if (param_5 != -1) {
      (*(float*)rtguest::G((uint32_t)fStack_70)) = (*(float*)rtguest::G((uint32_t)param_2));
      fVar5 = (*(uint32_t*)rtguest::G(0x332b90));
      fVar6 = (*(float*)rtguest::G(0x332b8c));
      while( true ) {
        pfVar1 = param_2 + 1 * sizeof(float);  // fn:29 float* elem steps (sig :29)
        pfVar2 = param_2 + 2 * sizeof(float);  // float* elem step
        pfVar3 = param_2 + 3 * sizeof(float);  // float* elem step
        param_2 = param_2 + 4 * sizeof(float);  // float* elem step
        (*(float*)rtguest::G((uint32_t)fStack_70)) = (*(float*)rtguest::G((uint32_t)fStack_70)) - (*(float*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x90)));
        (*(float*)rtguest::G((uint32_t)fStack_6c)) = (*(float*)rtguest::G((uint32_t)pfVar1)) - (*(float*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x94)));
        (*(float*)rtguest::G((uint32_t)fStack_68)) = (*(float*)rtguest::G((uint32_t)pfVar2)) - (*(float*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x98)));
        if (((*(float*)rtguest::G((uint32_t)pfVar3)) <= param_1) && (0.0 <= (*(float*)rtguest::G((uint32_t)fStack_6c)))) {
          RT_CALLRA(0x219d88, guest::vu0ApplyMatrix(fStack_70,param_4,fStack_70));
          if (((*(float*)rtguest::G((uint32_t)fStack_68)) < 0.0) || (1.5 < (*(float*)rtguest::G((uint32_t)fStack_68)))) {
            if ((-1.5 <= (*(float*)rtguest::G((uint32_t)fStack_68))) && ((*(float*)rtguest::G((uint32_t)fStack_68)) < 0.0)) {
              if (((*(float*)rtguest::G((uint32_t)fStack_70)) < fVar6) || (0.0 <= (*(float*)rtguest::G((uint32_t)fStack_70)))) {
                if ((0.0 <= (*(float*)rtguest::G((uint32_t)fStack_70))) && ((*(float*)rtguest::G((uint32_t)fStack_70)) <= fVar5)) {
                  uVar4 = uVar4 | 8;
                }
              }
              else {
                uVar4 = uVar4 | 4;
              }
            }
          }
          else if (((*(float*)rtguest::G((uint32_t)fStack_70)) < fVar6) || (0.0 <= (*(float*)rtguest::G((uint32_t)fStack_70)))) {
            if ((0.0 <= (*(float*)rtguest::G((uint32_t)fStack_70))) && ((*(float*)rtguest::G((uint32_t)fStack_70)) <= fVar5)) {
              uVar4 = uVar4 | 2;
            }
          }
          else {
            uVar4 = uVar4 | 1;
          }
        }
        param_5 = param_5 + -1;
        if (param_5 == -1) break;
        (*(float*)rtguest::G((uint32_t)fStack_70)) = (*(float*)rtguest::G((uint32_t)param_2));
      }
    }
  }
  return uVar4;
}


RT_GUEST_FN(0x00219cb0, guest::TestQuadLateralMask)

}  // namespace guest
