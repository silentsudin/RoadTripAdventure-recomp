// guest:0x00271cd8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// sig: param_2/5/6 long/ulong (fn/00271cd8.c:8-9; param_5 >> 8 loop :19, (param_6 & 0xffffffff) << 9 :25), uint64_t
uint32_t gifEmitBlock8Planes(uint32_t param_1, uint64_t param_2, uint32_t param_3, uint32_t param_4, uint64_t param_5, uint64_t param_6, ...) {
  rtguest::FrameGuard rt_fg(160);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x90) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x94) = 0x00000000u;
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
