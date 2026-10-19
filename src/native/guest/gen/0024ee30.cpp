// guest:0x0024ee30 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiArenaRotQuads2(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
    uint32_t fStack_d0 = rtguest::allocFrame(4);
    uint32_t fStack_cc = rtguest::allocFrame(4);
    rtguest::allocFrame(12);  // guest frame gap
    uint32_t fStack_bc = rtguest::allocFrame(4);
    rtguest::allocFrame(12);  // guest frame gap
    uint32_t fStack_ac = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
    uint32_t fStack_a0 = rtguest::allocFrame(4);
    uint32_t fStack_9c = rtguest::allocFrame(4);
    uint32_t fStack_98 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
uint32_t auStack_90 = rtguest::allocFrame(16);
uint32_t afStack_80 = rtguest::allocFrame(16);
    uint32_t iStack_70 = rtguest::allocFrame(4);
uint32_t pfVar1;
  int iVar2;
  int iVar3;
  float fVar4;
  
  if (0 < param_3) {
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t)));
    (*(int32_t*)rtguest::G((uint32_t)iStack_70)) = param_4;
    while( true ) {
      if ((*(int32_t*)rtguest::G((uint32_t)param_1)) < iVar3 + 0xa0) {
        (*(int32_t*)rtguest::G((uint32_t)param_1)) = 0;
        iVar3 = 0;
      }
      else {
        (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t))) = iVar3 + 0xa0;
        iVar3 = (int)param_1 + iVar3 + 0x10;
      }
      if (iVar3 == 0) break;
      gifQuadTag(iVar3);
      uiSetQuadXY(iVar3,(short)(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(4) * sizeof(int32_t))),(short)(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(5) * sizeof(int32_t))),(short)(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t))),(short)(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(3) * sizeof(int32_t))));
      uiSetSpriteClamp(iVar3 + 0x10,(short)(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(4) * sizeof(int32_t))),(short)(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(5) * sizeof(int32_t))),(short)(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t))),
                       (short)(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(3) * sizeof(int32_t))));
      vu0UnitMatrix(fStack_d0);
      vu0CopyVector(afStack_80,param_2 + 0x1c);
      iVar2 = 2;
      pfVar1 = afStack_80;
      do {
        fVar4 = (*(float*)rtguest::G((uint32_t)pfVar1));
        if (1.0 < fVar4) {
          fVar4 = ((fVar4 - 1.0) - (float)((int)(fVar4 - 1.0) / 2 << 1)) - 1.0;
LAB_0024efbc:
          (*(float*)rtguest::G((uint32_t)pfVar1)) = fVar4;
        }
        else if (fVar4 < -1.0) {
          fVar4 = ((fVar4 + 1.0) - (float)((int)(fVar4 + 1.0) / 2 << 1)) + 1.0;
          goto LAB_0024efbc;
        }
        iVar2 = iVar2 + -1;
        pfVar1 = pfVar1 + 1;
      } while (-1 < iVar2);
      sceVu0ScaleVectorXYZ((*(uint32_t*)rtguest::G(0x332f8c)),afStack_80,afStack_80);
      vu0RotMatrix(fStack_d0,fStack_d0,afStack_80);
      (*(float*)rtguest::G((uint32_t)fStack_cc)) = (*(float*)rtguest::G((uint32_t)fStack_cc)) * 0.5;
      (*(float*)rtguest::G((uint32_t)fStack_98)) = ((*(uint32_t*)rtguest::G(0x332f90)) - (float)((*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(8) * sizeof(int32_t))) + param_6)) * 0.0625;
      (*(float*)rtguest::G((uint32_t)fStack_bc)) = (*(float*)rtguest::G((uint32_t)fStack_bc)) * 0.5;
      (*(float*)rtguest::G((uint32_t)fStack_ac)) = (*(float*)rtguest::G((uint32_t)fStack_ac)) * 0.5;
      (*(float*)rtguest::G((uint32_t)fStack_d0)) = (*(float*)rtguest::G((uint32_t)fStack_d0)) + (*(float*)rtguest::G((uint32_t)fStack_d0));
      (*(float*)rtguest::G((uint32_t)fStack_a0)) = (float)((*(int32_t*)rtguest::G((uint32_t)param_2)) + (*(int32_t*)rtguest::G((uint32_t)iStack_70)) + 0x6c0);
      (*(float*)rtguest::G((uint32_t)fStack_9c)) = (float)(((*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t))) + param_5) / 2 + 0x790);
      vu0ApplyMatrix(auStack_90,fStack_d0,param_2 + 0xc);
      vu0Ftoi4Vector(iVar3 + 0x30,auStack_90);
      vu0ApplyMatrix(auStack_90,fStack_d0,param_2 + 0x10);
      vu0Ftoi4Vector(iVar3 + 0x50,auStack_90);
      vu0ApplyMatrix(auStack_90,fStack_d0,param_2 + 0x14);
      vu0Ftoi4Vector(iVar3 + 0x70,auStack_90);
      vu0ApplyMatrix(auStack_90,fStack_d0,param_2 + 0x18);
      param_2 = param_2 + 0x20;
      vu0Ftoi4Vector(iVar3 + 0x90,auStack_90);
      if (param_3 < 5) {
        return 0;
      }
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t)));
    }
  }
  return 0;
}


RT_GUEST_FN(0x0024ee30, guest::uiArenaRotQuads2)

}  // namespace guest
