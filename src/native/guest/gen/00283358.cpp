// guest:0x00283358 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _setlocale_r(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
  int iVar1;
uint32_t __s1;
  
  if (param_3 != 0) {
    __s1 = (uint32_t)param_3;
    iVar1 = ::strcmp((char *)rtguest::G((uint32_t)(__s1)), (char *)rtguest::G((uint32_t)(0x200022u)));
    if ((iVar1 != 0) && (iVar1 = ::strcmp((char *)rtguest::G((uint32_t)(__s1)), (char *)rtguest::G((uint32_t)(0x1ff007u))), iVar1 != 0)) {
      return (uint32_t)0x0;
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x34))) = __s1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x30))) = param_2;
  }
  return 0x00332418;
}


RT_GUEST_FN(0x00283358, guest::_setlocale_r)

}  // namespace guest
