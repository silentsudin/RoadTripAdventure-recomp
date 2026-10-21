// guest:0x0025cc18 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t drawFieldProp28Batch(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(160);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x78, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0x80, 8);
    rtguest::rtSpillF(rt_fg, 4, rt_sv, 0x90);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x48, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x58, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x68, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x70, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x88) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x8c) = 0x00000000u;
    rtguest::setS32(7, (uint32_t)param_2);  // walk: = param_2
  }
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
  RT_CALLRA(0x25cc70, guest::vifFixedRegs(0x2005880559306c38,7,0,0xf));
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
        fVar10 = (*(float*)rtguest::G((uint32_t)pfVar4)) - rtguest::fdiv(fVar11, 30.0);
        (*(float*)rtguest::G((uint32_t)pfVar4)) = fVar10;
        if (fVar10 < -fVar11) {
          bVar2 = bVar2 | 0x80;
          (*(int8_t*)rtguest::G((uint32_t)pbVar5)) = bVar2;
        }
        if ((bVar2 & 0x80) == 0) goto LAB_0025cd60;
        fVar11 = (*(float*)rtguest::G((uint32_t)pfVar6));
      }
      fVar11 = (*(float*)rtguest::G((uint32_t)pfVar4)) + rtguest::fdiv(fVar11, 30.0);
      (*(float*)rtguest::G((uint32_t)pfVar4)) = fVar11;
      if (0.0 <= fVar11) {
        (*(int8_t*)rtguest::G((uint32_t)pbVar5)) = (*(int8_t*)rtguest::G((uint32_t)pbVar5)) & 0x7f;
      }
    }
LAB_0025cd60:
    iVar9 = iVar9 + 1;
    RT_CALLRA(0x25cd60, guest::vu0UnitMatrix(auStack_a0));
    fVar11 = (*(float*)rtguest::G((uint32_t)pfVar4));
    pbVar5 = pbVar5 + 1;
    pfVar6 = pfVar6 + 0x4;  // T* element step
    pfVar4 = pfVar4 + 0x4;  // T* element step
    RT_CALLRA(0x25cd98, guest::vu0RotMatrixX(rtguest::fdiv((fVar11 * (*(float*)rtguest::G(0x3330d4))), 180.0),auStack_a0,auStack_a0));
    RT_CALLRA(0x25cdb0, guest::tileSlotOrigin(auStack_70,iVar8 * 0x10 + 0x2c43c0,0));
    RT_CALLRA(0x25cdb8, guest::vifObjectMatrix(auStack_a0));
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)piVar7));
    piVar7 = piVar7 + 0x4;  // T* element step
    RT_CALLRA(0x25cdc8, guest::vifCallModel(iVar1 + 0x10));
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
