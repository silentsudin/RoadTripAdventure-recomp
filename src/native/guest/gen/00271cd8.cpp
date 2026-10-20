// guest:0x00271cd8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gifEmitBlock8Planes(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
  rtguest::FrameGuard rt_fg(160);
  ulong uVar1;
  int iVar2;
  
  if (param_5 != 0) {
    param_2 = param_2 - (uint32_t)(param_3 * 0x10000 + 0x40000);
    iVar2 = 7;
    do {
      uVar1 = param_5 & 0xff;
      param_2 = param_2 + (uint32_t)(uint32_t)(param_3 * 0x10000);
      param_5 = param_5 >> 8;
      if (uVar1 != 0) {
        gifWriteTag(param_1,0xc400000000008000,(*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x003309a0 + (int)uVar1 * 8))));
        gifPkWriteQword(param_1,(param_6 & 0xffffffff) << 9 | 0xc2);
        gifPkWriteQword(param_1,param_4);
        gifPkWriteQword(param_1,param_2);
        gifPkWriteQword(param_1,param_2 + 0x10);
        gifPkWriteQword(param_1,param_2 + 0x20);
        gifPkWriteQword(param_1,param_2 + 0x30);
        gifPkWriteQword(param_1,param_2 + 0x40);
        gifPkWriteQword(param_1,param_2 + 0x50);
        gifPkWriteQword(param_1,param_2 + 0x60);
        gifPkWriteQword(param_1,param_2 + 0x70);
        gifPkWriteQword(param_1,param_2 + 0x80);
        gifPkWriteQword(param_1,param_2 + 0x90);
        sceGifPkCloseGifTag(param_1);
      }
      iVar2 = iVar2 + -1;
    } while (iVar2 != -1);
  }
  return 0;
}


RT_GUEST_FN(0x00271cd8, guest::gifEmitBlock8Planes)

}  // namespace guest
