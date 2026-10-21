// guest:0x00284838 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __sseek(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
  }
  long lVar1;
  ushort uVar2;
  
  lVar1 = RT_CALLRA(0x284854, guest::_lseek_r((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x54))),(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xe))),param_2,param_3));
  if (lVar1 == -1) {
    uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xc))) & 0xefff;
  }
  else {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x50))) = (int)lVar1;
    uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xc))) | 0x1000;
  }
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xc))) = uVar2;
  return 0;
}


RT_GUEST_FN(0x00284838, guest::__sseek)

}  // namespace guest
