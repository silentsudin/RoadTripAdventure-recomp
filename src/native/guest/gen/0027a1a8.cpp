// guest:0x0027a1a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceMcSync(uint64_t param_1, uint64_t param_2, uint64_t param_3, ...) {
  rtguest::FrameGuard rt_fg(112);
  long lVar1;
  long lVar2;
  uint uVar3;
  
  if ((*(int32_t*)rtguest::G(0x2e89b8)) == 0) {
    uVar3 = 0xffffffff;
  }
  else {
    lVar1 = RT_CALLRA(0x27a1ec, guest::sceSifCheckStatRpc(0x1774b40));
    if ((param_1 == 0) && (lVar1 != 0)) {
      while( true ) {
        lVar2 = RT_CALLRA(0x27a218, guest::sceSifCheckStatRpc(0x1774b40));
        lVar1 = 0;
        if (lVar2 == 0) break;
        RT_CALLRA(0x27a210, guest::mcDelayThread(0x3c));
      }
    }
    uVar3 = (uint32_t)(lVar1 == 0);
    if (param_2 != 0) {
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)param_2)) = (*(int32_t*)rtguest::G(0x2e89b8));
    }
    if (uVar3 != 0) {
      (*(int32_t*)rtguest::G(0x2e89b8)) = 0;
      if (param_3 != 0) {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)param_3)) = (*(uint32_t*)rtguest::G(0x1776100));
      }
      RT_CALLRA(0x27a258, guest::SignalSema((*(uint32_t*)rtguest::G(0x2e89bc))));
    }
  }
  return uVar3;
}


RT_GUEST_FN(0x0027a1a8, guest::sceMcSync)

}  // namespace guest
