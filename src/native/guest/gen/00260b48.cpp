// guest:0x00260b48 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t roulettePlaceBallOnWheel(uint32_t param_1, ...) {
uint32_t auStack_a0 = rtguest::allocFrame(64);
uint32_t auStack_60 = rtguest::allocFrame(4);
    uint32_t fStack_5c = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_50 = rtguest::allocFrame(8);
    uint32_t uStack_48 = rtguest::allocFrame(8);
  bool bVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  float fVar5;
  float fVar6;
  
  (*(uint64_t*)rtguest::G((uint32_t)uStack_48)) = 0x43c80000;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_50)) = 0x43c80000;
  vu0SubVector(uStack_50,0x177ace0,uStack_50);
  (*(uint64_t*)rtguest::G((uint32_t)uStack_50)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_50)) & 0xffffffff;
  fVar5 = rtguest::fbits(vecDotProductVu0(uStack_50,uStack_50));
  fVar5 = SQRT(fVar5);
  vecNormalizeVu0(uStack_50,uStack_50);
  if (fVar5 < 15.0) {
    fVar5 = fVar5 + (*(uint32_t*)rtguest::G(0x33317c));
    bVar1 = 15.0 < fVar5;
  }
  else {
    if (fVar5 <= 15.0) goto LAB_00260c44;
    fVar5 = fVar5 - (*(uint32_t*)rtguest::G(0x333180));
    bVar1 = fVar5 < 15.0;
  }
  if (bVar1) {
    fVar5 = 15.0;
  }
LAB_00260c44:
  vu0ScaleVector(fVar5,uStack_50,uStack_50);
  fVar5 = rtguest::fbits(atan2f(rtguest::f2bits((*(float*)rtguest::G(0x177ace0)) - 400.0f),rtguest::f2bits((*(float*)rtguest::G(0x177ace8)) - 400.0f)));
  iVar2 = -10;
  iVar3 = (int)(((((int)(short)(int)((fVar5 / (*(uint32_t*)rtguest::G(0x333184))) * 32768.0) -
                  (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xc)))) + 0x666 & 0xffff) % 0xccc + 0xf99a) * 0x10000)
          >> 0x10;
  if (iVar3 < 0xb) {
    iVar2 = 0;
  }
  iVar4 = 10;
  if (-0xb < iVar3) {
    iVar4 = iVar2;
  }
  vu0ScaleVector(0,auStack_60,auStack_60);
  (*(float*)rtguest::G((uint32_t)fStack_5c)) = (float)((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xe))) + iVar4) * (*(uint32_t*)rtguest::G(0x333188)) * 3.0517578e-05;
  vu0UnitMatrix(auStack_a0);
  vu0RotMatrix(auStack_a0,auStack_a0,auStack_60);
  vu0ApplyMatrix(uStack_50,auStack_a0,uStack_50);
  fVar5 = (float)(*(uint64_t*)rtguest::G((uint32_t)uStack_50)) + 400.0;
  fVar6 = (float)(*(uint64_t*)rtguest::G((uint32_t)uStack_48)) + 400.0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_50)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_50 + 4)),fVar5);
  (*(uint64_t*)rtguest::G((uint32_t)uStack_48)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_48 + 4)),fVar6);
  StoreIndexedPosition
            (fVar5,(*(uint32_t*)rtguest::G(0x177ace4)),fVar6,0,
             (int)(((uint32_t)(*(uint32_t*)rtguest::G(0x177ae24)) + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xe)))) * 0x10000) >> 0x10);
  return 0;
}


RT_GUEST_FN(0x00260b48, guest::roulettePlaceBallOnWheel)

}  // namespace guest
