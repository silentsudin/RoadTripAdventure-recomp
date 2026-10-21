// guest:0x00271e60 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t textEmitGlyphsToGs(uint64_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6) {
  rtguest::FrameGuard rt_fg(176);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0x90, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x80, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0xa0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xa4) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x70, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x60, 8);
    rtguest::setS32(0, (uint32_t)param_5);  // walk: = param_5
    rtguest::setS32(2, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(7, (uint32_t)param_1);  // walk: = param_1
    rtguest::gS[8] = 0x0000000000000000ull;  // walk const
  }
  ushort uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  ulong uVar6;
  uint uVar7;
  int iVar8;
  
  param_6 = param_6 + -1;
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(4) * sizeof(int32_t)));
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t)));
  iVar4 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(3) * sizeof(int32_t)));
  iVar5 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t)));
  uVar7 = (*(int32_t*)rtguest::G((uint32_t)param_2)) + param_3 * iVar3;
  RT_CALLRA(0x271ed4, guest::gifPkAppendTagNreg1(param_1,0,0,0));
  iVar8 = 0;
  if (param_6 != -1) {
    uVar1 = (*(uint16_t*)rtguest::G((uint32_t)param_5));
    iVar8 = 0;
    while ((uVar1 & 0xff) != 0) {
      param_5 = param_5 + 1;
      uVar6 = (uint32_t)uVar7;
      uVar7 = uVar7 + iVar3;
      iVar8 = iVar8 + 1;
      RT_CALLRA(0x271f58, guest::gifEmitBlock8Planes(param_1,uVar6 | (uint32_t)(uint32_t)(iVar5 + param_4 * iVar4) << 0x10 |
                                  0xffffffff00000000,iVar2,(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)((uVar1 >> 8 & 7) + 8) * sizeof(int32_t))),
                          (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x00330190 + (uVar1 & 0xff) * 8))),(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(5) * sizeof(int32_t)))));
      param_6 = param_6 + -1;
      if (param_6 == -1) {
        return iVar8;
      }
      uVar1 = (*(uint16_t*)rtguest::G((uint32_t)param_5));
    }
  }
  return iVar8;
}


RT_GUEST_FN(0x00271e60, guest::textEmitGlyphsToGs)

}  // namespace guest
