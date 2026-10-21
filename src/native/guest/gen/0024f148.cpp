// guest:0x0024f148 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiArenaFlatRects(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x28, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x38, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x20, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x40) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x44) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(2, (uint32_t)param_3);  // walk: = param_3
    rtguest::setS32(5, (uint32_t)param_6);  // walk: = param_6
    rtguest::setS32(6, (uint32_t)param_5);  // walk: = param_5
    rtguest::setS32(7, (uint32_t)param_4);  // walk: = param_4
  }
uint32_t puVar1;
uint32_t puVar2;
uint32_t puVar3;
uint32_t puVar4;
uint32_t puVar5;
uint32_t puVar6;
  ushort uVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  
  iVar8 = (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t)));
  if (((*(int32_t*)rtguest::G((uint32_t)param_1)) - iVar8) / 0x50 < 1) {
    (*(int32_t*)rtguest::G((uint32_t)param_1)) = 0;
  }
  else {
    if ((*(int32_t*)rtguest::G((uint32_t)param_1)) < iVar8 + 0x10) {
      (*(int32_t*)rtguest::G((uint32_t)param_1)) = 0;
      iVar8 = 0;
    }
    else {
      (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t))) = iVar8 + 0x10;
      iVar8 = (int)param_1 + iVar8 + 0x10;
    }
    if (iVar8 != 0) {
      iVar10 = 0;
      iVar9 = ((*(int32_t*)rtguest::G((uint32_t)param_1)) - (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t)))) / 0x20;
      if (iVar9 < param_3) {
        param_3 = iVar9;
      }
      RT_CALLRA(0x24f208, guest::gifFlatRectTag(iVar8,param_3));
      if (0 < param_3) {
        iVar8 = (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t)));
        while( true ) {
          iVar9 = (int)param_1 + iVar8 + 0x10;
          if ((*(int32_t*)rtguest::G((uint32_t)param_1)) < iVar8 + 0x20) {
            (*(int32_t*)rtguest::G((uint32_t)param_1)) = 0;
            iVar9 = 0;
          }
          else {
            (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t))) = iVar8 + 0x20;
          }
          uVar7 = (*(uint16_t*)rtguest::G((uint32_t)param_2));
          iVar10 = iVar10 + 1;
          puVar1 = param_2 + 2 * sizeof(uint16_t);  // fn:54 ushort* elem steps (sig :7)
          puVar2 = param_2 + 4 * sizeof(uint16_t);  // ushort* elem step
          puVar3 = param_2 + 8 * sizeof(uint16_t);  // ushort* elem step
          puVar4 = param_2 + 10 * sizeof(uint16_t);  // ushort* elem step
          puVar5 = param_2 + 0xc * sizeof(uint16_t);  // ushort* elem step
          puVar6 = param_2 + 0xe * sizeof(uint16_t);  // ushort* elem step
          param_2 = param_2 + 0x10 * sizeof(uint16_t);  // ushort* elem step
          RT_CALLRA(0x24f288, guest::uiFlatVertexWrite(iVar9,(int)(((uint32_t)uVar7 + param_4) * 0x10000) >> 0x10,
                            (int)(((uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar1)) + param_5) * 0x10000) >> 0x10,
                            (int)(((uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar2)) + param_6) * 0x10000) >> 0x10,(*(uint16_t*)rtguest::G((uint32_t)puVar3)),(*(uint16_t*)rtguest::G((uint32_t)puVar4)),
                            (*(uint16_t*)rtguest::G((uint32_t)puVar5)),(*(uint16_t*)rtguest::G((uint32_t)puVar6))));
          if (param_3 <= iVar10) break;
          iVar8 = (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t)));
        }
      }
    }
  }
  return 0;
}


RT_GUEST_FN(0x0024f148, guest::uiArenaFlatRects)

}  // namespace guest
