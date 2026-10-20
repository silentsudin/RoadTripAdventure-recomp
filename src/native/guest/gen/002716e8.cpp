// guest:0x002716e8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

void textGridFillRectBlank(uint64_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5) {
  rtguest::FrameGuard rt_fg(144);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x80) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x84) = 0x00000000u;
  }
  int iVar1;
  uint uVar2;
  uint uVar3;
  
  uVar2 = 0;
  if (param_5 != 0) {
    do {
      uVar3 = 0;
      if (param_4 != 0) {
        iVar1 = param_2;
        do {
          RT_CALLRA(0x271750, guest::textBufferSetPixel16(param_1,iVar1,param_3 + uVar2,0x720));
          uVar3 = uVar3 + 1;
          iVar1 = param_2 + uVar3;
        } while (uVar3 < param_4);
      }
      uVar2 = uVar2 + 1;
    } while (uVar2 < param_5);
  }
  return;
}


RT_GUEST_FN(0x002716e8, guest::textGridFillRectBlank)

}  // namespace guest
