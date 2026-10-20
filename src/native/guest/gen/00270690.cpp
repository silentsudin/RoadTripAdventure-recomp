// guest:0x00270690 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceDmaSendI(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x50) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x54) = 0x00000000u;
  }
  int iVar1;
  
  if (((*(uint32_t*)rtguest::G((uint32_t)param_1)) & 0x100) != 0) {
    iVar1 = 0xffffff;
    do {
      if (iVar1 < 0) {
        rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x330178u)));
        if (((*(uint32_t*)rtguest::G((uint32_t)param_1)) >> 8 & 1) != 0) {
          (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1)) & 0xfffffeff;
        }
      }
      iVar1 = iVar1 + -1;
    } while (((*(uint32_t*)rtguest::G((uint32_t)param_1)) & 0x100) != 0);
  }
  if ((*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(uint32_t))) != 0xffffffff) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(uint32_t))) = param_2;
  }
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(8) * sizeof(uint32_t))) = param_3;
  (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1)) & 0xfffffff3 | 0x109;
  return 0;
}


RT_GUEST_FN(0x00270690, guest::sceDmaSendI)

}  // namespace guest
