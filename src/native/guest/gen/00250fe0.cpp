// guest:0x00250fe0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CalculateBallLaunchImpulse(uint32_t param_1, ...) {
    uint32_t fStack_2c = rtguest::allocFrame(4);
  float fVar1;
  undefined4 uVar2;
  float fVar3;
uint32_t auStack_30 = rtguest::allocFrame(4);
  
  vu0SubVector(auStack_30,param_1 + 0x60,0x177ace0);
  fVar3 = (float)vecDotProductVu0(auStack_30,auStack_30);
  uVar2 = 0;
  if (SQRT(fVar3) < 2.0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x88))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8c))) = 0;
    vu0Itof0Vector(auStack_30,0x177ad40);
    vu0ScaleVector(0x38000000,auStack_30,auStack_30);
    fVar3 = (float)vecDotProductVu0(auStack_30,auStack_30);
    fVar3 = SQRT(fVar3);
    if (fVar3 < (*(float*)rtguest::G(0x332fb4))) {
      vecNormalizeVu0(auStack_30,auStack_30);
      vu0ScaleVector(fVar3 * (*(uint32_t*)rtguest::G(0x332fb8)) + (*(uint32_t*)rtguest::G(0x332fbc)),auStack_30,auStack_30);
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x84))) = 0x318;
    }
    else {
      fVar3 = fVar3 - (*(float*)rtguest::G(0x332fb4));
      vu0ScaleVector((*(uint32_t*)rtguest::G(0x332fc0)),auStack_30,auStack_30);
      (*(float*)rtguest::G((uint32_t)fStack_2c)) = (*(uint32_t*)rtguest::G(0x332fcc));
      fVar1 = (*(uint32_t*)rtguest::G(0x332fc8));
      if ((*(uint32_t*)rtguest::G(0x332fc4)) <= fVar3) {
        fVar3 = (*(uint32_t*)rtguest::G(0x332fc4));
      }
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x84))) = 0x317;
      (*(float*)rtguest::G((uint32_t)fStack_2c)) = ((fVar3 * 180.0) / fVar1) * (*(float*)rtguest::G((uint32_t)fStack_2c));
    }
    vu0CopyVector(param_1 + 0x70,auStack_30);
    uVar2 = 1;
  }
  return uVar2;
}


RT_GUEST_FN(0x00250fe0, guest::CalculateBallLaunchImpulse)

}  // namespace guest
