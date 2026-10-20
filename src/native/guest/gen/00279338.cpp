// guest:0x00279338 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSdTransToIOP(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
  long lVar1;
  long lVar2;
  
  if (param_4 != 0) {
    (*(uint32_t*)rtguest::G(0x177384c)) = 0;
    (*(uint32_t*)rtguest::G(0x1773840)) = param_1;
    (*(uint32_t*)rtguest::G(0x1773844)) = param_2;
    (*(uint32_t*)rtguest::G(0x1773848)) = param_3;
    lVar1 = RT_CALLRA(0x279370, guest::sceSifSetDma(0x1773840,1));
    if (lVar1 != 0) {
      if (param_4 != 1) {
        return 0;
      }
      do {
        lVar2 = (int32_t)RT_CALLRA(0x279398, guest::sceSifDmaStat(lVar1));
      } while (-1 < lVar2);
      return 0;
    }
  }
  return 0xffffffff;
}


RT_GUEST_FN(0x00279338, guest::sceSdTransToIOP)

}  // namespace guest
