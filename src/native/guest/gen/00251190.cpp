// guest:0x00251190 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ApplyCupAttraction(uint32_t param_1, ...) {
    uint32_t uStack_54 = rtguest::allocFrame(4);
    uint32_t uStack_58 = rtguest::allocFrame(4);
    uint32_t uStack_5c = rtguest::allocFrame(4);
    uint32_t uStack_60 = rtguest::allocFrame(4);
  undefined4 uVar1;
  undefined4 uVar2;
  float fVar3;
  float fVar4;
  
  vu0SubVector(uStack_60,param_1 + 0x60,0x177ace0);
  fVar3 = (float)vecDotProductVu0(uStack_60,uStack_60);
  if (SQRT(fVar3) < (*(float*)rtguest::G(0x332fd0))) {
    uVar2 = 0;
    if ((*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x74))) <= 0.0) {
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x88))) == 0) {
        uVar2 = 0;
      }
      else {
        vu0CopyVector(uStack_60,param_1 + 0x70);
        (*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) = 0;
        fVar3 = (float)vecDotProductVu0(uStack_60,uStack_60);
        if (SQRT(fVar3) == 0.0) {
          vu0SubVector(uStack_60,param_1 + 0x60,0x177ace0);
          (*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) = 0;
          fVar3 = (float)vecDotProductVu0(uStack_60,uStack_60);
          if (SQRT(fVar3) == 0.0) {
            fVar3 = (*(uint32_t*)rtguest::G(0x332fd8));
            (*(uint32_t*)rtguest::G((uint32_t)uStack_60)) = sinf((float)((*(uint32_t*)rtguest::G(0x177ae24)) + 0x8000) * (*(uint32_t*)rtguest::G(0x332fd8)) * 3.0517578e-05,
                             uStack_60);
            (*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) = 0;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_58)) = cosf((float)((*(uint32_t*)rtguest::G(0x177ae24)) + 0x8000) * fVar3 * 3.0517578e-05);
            (*(uint32_t*)rtguest::G((uint32_t)uStack_54)) = 0;
          }
          vecNormalizeVu0(uStack_60,uStack_60);
          vu0ScaleVector((*(uint32_t*)rtguest::G(0x332fdc)),uStack_60,uStack_60);
        }
        else {
          fVar4 = (float)vecDotProductVu0(uStack_60,uStack_60);
          fVar3 = (*(uint32_t*)rtguest::G(0x332fd4));
          if (SQRT(fVar4) < (*(uint32_t*)rtguest::G(0x332fd4))) {
            vecNormalizeVu0(uStack_60,uStack_60);
            vu0ScaleVector(fVar3,uStack_60,uStack_60);
          }
        }
        vu0CopyVector(param_1 + 0x70,uStack_60);
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
