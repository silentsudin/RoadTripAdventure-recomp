// guest:0x00253b80 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t HandleCarObjectCollisions(uint32_t param_1, uint32_t param_2, ...) {
uint32_t auStack_b0 = rtguest::allocFrame(16);
    uint32_t fStack_a0 = rtguest::allocFrame(4);
    uint32_t fStack_9c = rtguest::allocFrame(4);
    uint32_t fStack_98 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
uint32_t auStack_90 = rtguest::allocFrame(16);
uint32_t afStack_80 = rtguest::allocFrame(8);
    uint32_t fStack_78 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_70 = rtguest::allocFrame(4);
  ushort uVar1;
  undefined4 uVar2;
uint32_t pfVar3;
  uint uVar4;
uint32_t puVar5;
  float fVar6;
  
  uVar2 = (*(uint32_t*)rtguest::G(0x333054));
  uVar4 = 0;
  puVar5 = 0x177ac50;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_70)) = param_2;
  do {
    vu0SubVector(auStack_b0,param_1 + 0x20,puVar5 + 0x12);
    vu0MulVector(fStack_a0,auStack_b0,auStack_b0);
    if (SQRT((*(float*)rtguest::G((uint32_t)fStack_a0)) + (*(float*)rtguest::G((uint32_t)fStack_9c)) + (*(float*)rtguest::G((uint32_t)fStack_98))) < 2.5) {
      uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0x33)));
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x40))) = (int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0x33)));
      if (((int)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x44))) >> (uVar4 & 0x1f) & 1U) == 0) {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x44))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x44))) | 1 << (uVar4 & 0x1f);
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x4c))) = 1;
        if ((uVar1 & 0xfffd) == 0) {
          gs_controls_rumble_start((*(uint32_t*)rtguest::G((uint32_t)uStack_70)),uVar1 & 1,6,0x40);
        }
      }
      vecNormalizeVu0(auStack_b0,auStack_b0);
      pfVar3 = (uint32_t)(param_1 + 0x30);
      vu0MulVector(afStack_80,pfVar3,pfVar3);
      vu0ScaleVector(SQRT((*(float*)rtguest::G((uint32_t)afStack_80 + (uint32_t)(0) * 4)) + (*(float*)rtguest::G((uint32_t)fStack_78))) * (*(uint32_t*)rtguest::G(0x333058)),afStack_80,auStack_b0);
      vu0Itof0Vector(auStack_90,puVar5 + 0x1e);
      vu0ScaleVector((*(uint32_t*)rtguest::G(0x33305c)),auStack_90,auStack_90);
      vu0AddVector(afStack_80,afStack_80,auStack_90);
      (*(float*)rtguest::G((uint32_t)pfVar3)) = (*(float*)rtguest::G((uint32_t)afStack_80 + (uint32_t)(0) * 4));
      (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x38))) = (*(float*)rtguest::G((uint32_t)fStack_78));
      if (SQRT((*(float*)rtguest::G((uint32_t)fStack_a0)) + (*(float*)rtguest::G((uint32_t)fStack_98))) < 2.0) {
        if (SQRT((*(float*)rtguest::G((uint32_t)afStack_80 + (uint32_t)(0) * 4)) * (*(float*)rtguest::G((uint32_t)afStack_80 + (uint32_t)(0) * 4)) + (*(float*)rtguest::G((uint32_t)fStack_78)) * (*(float*)rtguest::G((uint32_t)fStack_78))) < (*(uint32_t*)rtguest::G(0x333060))) {
          vu0ScaleVector(afStack_80,auStack_b0);
          (*(float*)rtguest::G((uint32_t)pfVar3)) = (*(float*)rtguest::G((uint32_t)afStack_80 + (uint32_t)(0) * 4));
          (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x38))) = (*(float*)rtguest::G((uint32_t)fStack_78));
          goto LAB_00253e2c;
        }
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x34))) = uVar2;
      }
      else {
        fVar6 = rtguest::fbits(GetSpeedFromF15Vector(puVar5 + 0x1e));
        if (50.0 < fVar6) {
LAB_00253e2c:
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x34))) = uVar2;
        }
      }
    }
    else {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x44))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x44))) & ~(1 << (uVar4 & 0x1f));
    }
    uVar4 = uVar4 + 1;
    puVar5 = puVar5 + 0x270;  // T* element step
    if (5 < (int)uVar4) {
      return 0;
    }
  } while( true );
}


RT_GUEST_FN(0x00253b80, guest::HandleCarObjectCollisions)

}  // namespace guest
