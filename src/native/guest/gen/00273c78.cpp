// guest:0x00273c78 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t scePadSetReqState(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
  int iVar1;
  int iVar2;
  
  iVar1 = (*(uint32_t*)rtguest::G((uint32_t)0x1773220 + (uint32_t)((int)param_1 * 0x1c + param_2 * 7) * 4));
  if (iVar1 != 0) {
    iVar2 = RT_CALLRA(0x273cbc, guest::scePadGetDmaStr(param_1));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x71))) = param_3;
  }
  return iVar1 != 0;
}


RT_GUEST_FN(0x00273c78, guest::scePadSetReqState)

}  // namespace guest
