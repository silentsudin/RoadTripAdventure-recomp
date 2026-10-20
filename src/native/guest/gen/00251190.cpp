// guest:0x00251190 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ApplyCupAttraction(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x38) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x3c) = 0x00000000u;
  }
    uint32_t uStack_60 = rtguest::allocFrame(4);
    uint32_t uStack_5c = rtguest::allocFrame(4);
    uint32_t uStack_58 = rtguest::allocFrame(4);
    uint32_t uStack_54 = rtguest::allocFrame(4);
  undefined4 uVar1;
  undefined4 uVar2;
  float fVar3;
  float fVar4;
  
  RT_CALLRA(0x2511d8, guest::vu0SubVector(uStack_60,param_1 + 0x60,0x177ace0));
  fVar3 = rtguest::fbits(RT_CALLRA(0x2511e4, guest::vecDotProductVu0(uStack_60,uStack_60)));
  if (SQRT(fVar3) < (*(float*)rtguest::G(0x332fd0))) {
    uVar2 = 0;
    if ((*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x74))) <= 0.0) {
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x88))) == 0) {
        uVar2 = 0;
      }
      else {
        RT_CALLRA(0x251250, guest::vu0CopyVector(uStack_60,param_1 + 0x70));
        (*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) = 0;
        fVar3 = rtguest::fbits(RT_CALLRA(0x251260, guest::vecDotProductVu0(uStack_60,uStack_60)));
        if (SQRT(fVar3) == 0.0) {
          RT_CALLRA(0x25130c, guest::vu0SubVector(uStack_60,param_1 + 0x60,0x177ace0));
          (*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) = 0;
          fVar3 = rtguest::fbits(RT_CALLRA(0x2512a0, guest::vecDotProductVu0(uStack_60,uStack_60)));
          if (SQRT(fVar3) == 0.0) {
            fVar3 = (*(float*)rtguest::G(0x332fd8));
            (*(uint32_t*)rtguest::G((uint32_t)uStack_60)) = RT_CALLRA(0x251384, guest::sinf(rtguest::f2bits((float)((*(uint32_t*)rtguest::G(0x177ae24)) + 0x8000) * (*(float*)rtguest::G(0x332fd8)) * 3.0517578e-05f),
                             uStack_60));
            (*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) = 0;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_58)) = RT_CALLRA(0x2513ac, guest::cosf(rtguest::f2bits((float)((*(uint32_t*)rtguest::G(0x177ae24)) + 0x8000) * fVar3 * 3.0517578e-05f)));
            (*(uint32_t*)rtguest::G((uint32_t)uStack_54)) = 0;
          }
          RT_CALLRA(0x2512e8, guest::vecNormalizeVu0(uStack_60,uStack_60));
          RT_CALLRA(0x2512f8, guest::vu0ScaleVector((*(float*)rtguest::G(0x332fdc)),uStack_60,uStack_60));
        }
        else {
          fVar4 = rtguest::fbits(RT_CALLRA(0x25131c, guest::vecDotProductVu0(uStack_60,uStack_60)));
          fVar3 = (*(float*)rtguest::G(0x332fd4));
          if (SQRT(fVar4) < (*(float*)rtguest::G(0x332fd4))) {
            RT_CALLRA(0x2513c0, guest::vecNormalizeVu0(uStack_60,uStack_60));
            RT_CALLRA(0x2513d0, guest::vu0ScaleVector(fVar3,uStack_60,uStack_60));
          }
        }
        RT_CALLRA(0x2513dc, guest::vu0CopyVector(param_1 + 0x70,uStack_60));
        uVar1 = (*(uint32_t*)rtguest::G(0x332fe0));
        uVar2 = 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8c))) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x74))) = uVar1;
      }
    }
  }
  else {
    uVar2 = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x88))) = 1;
  }
  return uVar2;
}


RT_GUEST_FN(0x00251190, guest::ApplyCupAttraction)

}  // namespace guest
