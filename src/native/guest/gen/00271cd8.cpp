// guest:0x00271cd8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// sig: param_2/5/6 long/ulong (fn/00271cd8.c:8-9; param_5 >> 8 loop :19, (param_6 & 0xffffffff) << 9 :25), uint64_t
uint32_t gifEmitBlock8Planes(uint32_t param_1, uint64_t param_2, uint32_t param_3, uint32_t param_4, uint64_t param_5, uint64_t param_6, ...) {
  rtguest::FrameGuard rt_fg(160);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x90) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x94) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0x80, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x70, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x30, 8);
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
    rtguest::gS[3] = 0x0000000000000007ull;  // walk const
    rtguest::gS[7] = 0x00000000000000c2ull;  // walk const
    rtguest::gS[8] = 0x00000000003309a0ull;  // walk const
  }
  ulong uVar1;
  int iVar2;
  
  if (param_5 != 0) {
    param_2 = param_2 - (uint64_t)(int32_t)(param_3 * 0x10000 + 0x40000);
    iVar2 = 7;
    do {
      uVar1 = param_5 & 0xff;
      param_2 = param_2 + (uint64_t)(uint32_t)(param_3 * 0x10000);
      param_5 = param_5 >> 8;
      if (uVar1 != 0) {
        RT_CALLRA(0x271d7c, guest::gifWriteTag(param_1,0xc400000000008000,(*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x003309a0 + (int)uVar1 * 8)))));
        RT_CALLRA(0x271d88, guest::gifPkWriteQword(param_1,(param_6 & 0xffffffff) << 9 | 0xc2));
        RT_CALLRA(0x271d94, guest::gifPkWriteQword(param_1,param_4));
        RT_CALLRA(0x271da0, guest::gifPkWriteQword(param_1,param_2));
        RT_CALLRA(0x271dac, guest::gifPkWriteQword(param_1,param_2 + 0x10));
        RT_CALLRA(0x271db8, guest::gifPkWriteQword(param_1,param_2 + 0x20));
        RT_CALLRA(0x271dc4, guest::gifPkWriteQword(param_1,param_2 + 0x30));
        RT_CALLRA(0x271dd0, guest::gifPkWriteQword(param_1,param_2 + 0x40));
        RT_CALLRA(0x271ddc, guest::gifPkWriteQword(param_1,param_2 + 0x50));
        RT_CALLRA(0x271de8, guest::gifPkWriteQword(param_1,param_2 + 0x60));
        RT_CALLRA(0x271df4, guest::gifPkWriteQword(param_1,param_2 + 0x70));
        RT_CALLRA(0x271e00, guest::gifPkWriteQword(param_1,param_2 + 0x80));
        RT_CALLRA(0x271e0c, guest::gifPkWriteQword(param_1,param_2 + 0x90));
        RT_CALLRA(0x271e14, guest::sceGifPkCloseGifTag(param_1));
      }
      iVar2 = iVar2 + -1;
    } while (iVar2 != -1);
  }
  return 0;
}


RT_GUEST_FN(0x00271cd8, guest::gifEmitBlock8Planes)

}  // namespace guest
