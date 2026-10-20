// guest:0x0025cc18 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawFieldProp28Batch(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(160);
  int iVar1;
  byte bVar2;
  uint uVar3;
uint32_t pfVar4;
uint32_t pbVar5;
uint32_t pfVar6;
uint32_t piVar7;
  int iVar8;
  int iVar9;
  float fVar10;
  float fVar11;
uint32_t auStack_a0 = rtguest::allocFrame(48);
uint32_t auStack_70 = rtguest::allocFrame(16);
  
  iVar8 = 0;
  vifFixedRegs(0x2005880559306c38,7,0,0xf);
LAB_0025cc88:
  piVar7 = 0x177f580;
  pfVar4 = (uint32_t)(0x017608d0 + iVar8 * 3);
  iVar9 = 0;
  pbVar5 = 0x01760e20 + iVar8 * 3;
  pfVar6 = (uint32_t)0x002c43b0;
  uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28)));
  do {
    if ((uVar3 & 0x8000) == 0) {
      bVar2 = (*(int8_t*)rtguest::G((uint32_t)pbVar5));
      fVar11 = (*(float*)rtguest::G((uint32_t)pfVar6));
      if ((bVar2 & 0x80) == 0) {
        fVar10 = (*(float*)rtguest::G((uint32_t)pfVar4)) - fVar11 / 30.0;
        (*(float*)rtguest::G((uint32_t)pfVar4)) = fVar10;
        if (fVar10 < -fVar11) {
          bVar2 = bVar2 | 0x80;
          (*(int8_t*)rtguest::G((uint32_t)pbVar5)) = bVar2;
        }
        if ((bVar2 & 0x80) == 0) goto LAB_0025cd60;
        fVar11 = (*(float*)rtguest::G((uint32_t)pfVar6));
      }
      fVar11 = (*(float*)rtguest::G((uint32_t)pfVar4)) + fVar11 / 30.0;
      (*(float*)rtguest::G((uint32_t)pfVar4)) = fVar11;
      if (0.0 <= fVar11) {
        (*(int8_t*)rtguest::G((uint32_t)pbVar5)) = (*(int8_t*)rtguest::G((uint32_t)pbVar5)) & 0x7f;
      }
    }
LAB_0025cd60:
    iVar9 = iVar9 + 1;
    vu0UnitMatrix(auStack_a0);
    fVar11 = (*(float*)rtguest::G((uint32_t)pfVar4));
    pbVar5 = pbVar5 + 1;
    pfVar6 = pfVar6 + 0x4;  // T* element step
    pfVar4 = pfVar4 + 0x4;  // T* element step
    vu0RotMatrixX((fVar11 * (*(float*)rtguest::G(0x3330d4))) / 180.0,auStack_a0,auStack_a0);
    tileSlotOrigin(auStack_70,iVar8 * 0x10 + 0x2c43c0,0);
    vifObjectMatrix(auStack_a0);
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)piVar7));
    piVar7 = piVar7 + 0x4;  // T* element step
    vifCallModel(iVar1 + 0x10);
    if (2 < iVar9) break;
    uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28)));
  } while( true );
  iVar8 = iVar8 + 1;
  if (0x6a < iVar8) {
    return 0;
  }
  goto LAB_0025cc88;
}


RT_GUEST_FN(0x0025cc18, guest::drawFieldProp28Batch)

}  // namespace guest
