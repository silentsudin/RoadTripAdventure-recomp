// guest:0x00207280 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t FindWorldMeshIntersection(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, ...) {
  rtguest::FrameGuard rt_fg(128);
    uint32_t fStack_80 = rtguest::allocFrame(4);
    uint32_t fStack_7c = rtguest::allocFrame(4);
    uint32_t fStack_78 = rtguest::allocFrame(4);
    uint32_t fStack_74 = rtguest::allocFrame(4);
    uint32_t fStack_70 = rtguest::allocFrame(4);
    uint32_t uStack_6c = rtguest::allocFrame(4);
    uint32_t fStack_68 = rtguest::allocFrame(4);
    uint32_t fStack_64 = rtguest::allocFrame(4);
    uint32_t uStack_60 = rtguest::allocFrame(4);
uint32_t afStack_5c = rtguest::allocFrame(12);
  long lVar1;
  long lVar2;
  int iVar3;
  int iVar4;
  int iVar5;
uint32_t piVar6;
uint32_t pfVar7;
  float fVar8;
  
  vu0CopyVector(fStack_80,param_1);
  pfVar7 = (uint32_t)param_1;
  if ((*(float*)rtguest::G((uint32_t)pfVar7 + (uint32_t)(2) * sizeof(float))) < 0.0) {
    if ((*(float*)rtguest::G((uint32_t)pfVar7)) < 800.0) {
      param_4 = param_4 - 1;
      (*(float*)rtguest::G((uint32_t)fStack_80)) = (*(float*)rtguest::G((uint32_t)fStack_80)) + 800.0;
    }
    else {
      param_4 = param_4 + 1;
      (*(float*)rtguest::G((uint32_t)fStack_80)) = (*(float*)rtguest::G((uint32_t)fStack_80)) - 800.0;
    }
    param_4 = param_4 & 3;
    (*(float*)rtguest::G((uint32_t)fStack_78)) = (*(float*)rtguest::G((uint32_t)fStack_78)) + 1600.0;
  }
  else {
    fVar8 = (*(float*)rtguest::G((uint32_t)pfVar7));
    if (1600.0 <= (*(float*)rtguest::G((uint32_t)pfVar7 + (uint32_t)(2) * sizeof(float)))) {
      if (fVar8 < 800.0) {
        param_4 = param_4 - 1;
        (*(float*)rtguest::G((uint32_t)fStack_80)) = (*(float*)rtguest::G((uint32_t)fStack_80)) + 800.0;
      }
      else {
        param_4 = param_4 + 1;
        (*(float*)rtguest::G((uint32_t)fStack_80)) = (*(float*)rtguest::G((uint32_t)fStack_80)) - 800.0;
      }
      param_4 = param_4 & 3;
      (*(float*)rtguest::G((uint32_t)fStack_78)) = (*(float*)rtguest::G((uint32_t)fStack_78)) - 1600.0;
    }
    else {
      if (fVar8 < 0.0) {
        (*(float*)rtguest::G((uint32_t)fStack_80)) = (*(float*)rtguest::G((uint32_t)fStack_80)) + 1600.0;
      }
      else {
        if (fVar8 < 1600.0) goto LAB_00207404;
        (*(float*)rtguest::G((uint32_t)fStack_80)) = (*(float*)rtguest::G((uint32_t)fStack_80)) - 1600.0;
      }
      param_4 = param_4 ^ 2;
    }
  }
LAB_00207404:
  iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 * 4 + param_5 + 0x60)));
  lVar1 = -1;
  if (-1 < iVar4) {
    iVar3 = ((int)((*(float*)rtguest::G((uint32_t)fStack_80)) / 100.0) & 0xfU) + ((int)((*(float*)rtguest::G((uint32_t)fStack_78)) / 100.0) & 0xfU) * 0x10;
    if (iVar4 == 0xb) {
      (*(float*)rtguest::G((uint32_t)fStack_70)) = (*(float*)rtguest::G(0x332b00));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_6c)) = (*(uint32_t*)rtguest::G(0x332b04));
      (*(float*)rtguest::G((uint32_t)fStack_68)) = 0.0;
      lVar1 = IntersectPointWithMesh
                        ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 * 4 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_5 + 0x7c)))))),fStack_80,param_3,
                         (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 * 2 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_5 + 0x7c))) + 0x400))),fStack_70,
                         uStack_6c,fStack_68,0xffffffffffffffff);
      piVar6 = (uint32_t)(param_5 + 0x80);
      iVar4 = 0;
      do {
        iVar5 = iVar4 + 1;
        lVar2 = TestProgressFlag(iVar4);
        if (lVar2 != 0) {
          lVar1 = IntersectPointWithMesh
                            ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 * 4 + (*(int32_t*)rtguest::G((uint32_t)piVar6))))),fStack_80,param_3,
                             (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 * 2 + (*(int32_t*)rtguest::G((uint32_t)piVar6)) + 0x400))),fStack_70,uStack_6c,
                             fStack_68,lVar1);
        }
        piVar6 = piVar6 + 0x4;  // T* element step
        iVar4 = iVar5;
      } while (iVar5 < 0x1a);
      (*(uint32_t*)rtguest::G((uint32_t)uStack_60)) = (*(uint32_t*)rtguest::G((uint32_t)uStack_6c));
      if (-1 < lVar1) {
        (*(float*)rtguest::G((uint32_t)fStack_7c)) = (*(float*)rtguest::G((uint32_t)fStack_70));
        (*(float*)rtguest::G((uint32_t)fStack_74)) = (*(float*)rtguest::G((uint32_t)fStack_68));
      }
    }
    else {
      iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 * 4 + param_5 + 0x70)));
      (*(float*)rtguest::G((uint32_t)fStack_64)) = (*(uint32_t*)rtguest::G(0x332b08));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_60)) = (*(uint32_t*)rtguest::G(0x332b0c));
      (*(float*)rtguest::G((uint32_t)afStack_5c + (uint32_t)(0) * 4)) = 0.0;
      lVar1 = IntersectPointWithMesh
                        ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 * 4 + iVar4))),fStack_80,param_3,
                         (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 * 2 + iVar4 + 0x400))),fStack_64,uStack_60,afStack_5c
                         ,0xffffffffffffffff);
      if (-1 < lVar1) {
        (*(float*)rtguest::G((uint32_t)fStack_7c)) = (*(float*)rtguest::G((uint32_t)fStack_64));
        (*(float*)rtguest::G((uint32_t)fStack_74)) = (*(float*)rtguest::G((uint32_t)afStack_5c + (uint32_t)(0) * 4));
      }
    }
    (*(uint32_t*)rtguest::G((uint32_t)param_2)) = (*(uint32_t*)rtguest::G((uint32_t)uStack_60));
    (*(float*)rtguest::G((uint32_t)pfVar7 + (uint32_t)(1) * sizeof(float))) = (*(float*)rtguest::G((uint32_t)fStack_7c));
    (*(float*)rtguest::G((uint32_t)pfVar7 + (uint32_t)(3) * sizeof(float))) = (*(float*)rtguest::G((uint32_t)fStack_74));
  }
  return lVar1;
}


RT_GUEST_FN(0x00207280, guest::FindWorldMeshIntersection)

}  // namespace guest
