// guest:0x0027a468 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int64_t sceMcGetDir(uint32_t param_1, uint32_t param_2, int64_t param_3, uint32_t param_4, int64_t param_5, uint64_t param_6, ...) {
  long lVar1;
  
  lVar1 = PollSema((*(uint32_t*)rtguest::G(0x2e89bc)));
  if (lVar1 < 0) {
    lVar1 = -200;
  }
  else if ((*(uint32_t*)rtguest::G(0x1774b64)) == 0) {
    SignalSema((*(uint32_t*)rtguest::G(0x2e89bc)));
    lVar1 = -100;
  }
  else if ((param_3 == 0) || ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)param_3)) == '\0')) {
    SignalSema((*(uint32_t*)rtguest::G(0x2e89bc)));
    lVar1 = -0xd2;
  }
  else {
    (*(uint32_t*)rtguest::G(0x1774c00)) = (uint32_t)param_6;
    (*(uint32_t*)rtguest::G(0x1774bf0)) = param_1;
    (*(uint32_t*)rtguest::G(0x1774bf4)) = param_2;
    (*(uint32_t*)rtguest::G(0x1774bf8)) = param_4;
    (*(uint32_t*)rtguest::G(0x1774bfc)) = (int)param_5;
    (uint32_t)(uintptr_t)(::strncpy((char *)rtguest::G((uint32_t)(0x01774c04)), (char *)rtguest::G((uint32_t)((uint32_t)param_3)), 0x3ff));
    (*(uint8_t*)rtguest::G(0x1775003)) = 0;
    if (-1 < param_5) {
      sceSifWriteBackDCache(param_6,(int)param_5 << 6);
    }
    lVar1 = sceSifCallRpc(0x1774b40,0xd,1,0x1774bf0,0x414,0x1776100,4,0);
    if (lVar1 == 0) {
      (*(uint32_t*)rtguest::G(0x2e89b8)) = 0xd;
    }
    else {
      SignalSema((*(uint32_t*)rtguest::G(0x2e89bc)));
    }
  }
  return lVar1;
}


RT_GUEST_FN(0x0027a468, guest::sceMcGetDir)

}  // namespace guest
