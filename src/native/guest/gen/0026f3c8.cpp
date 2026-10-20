// guest:0x0026f3c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiEnvCenterPosB(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg;
  ulong uVar1;
  long lVar2;
  
  lVar2 = (long)param_3 -
          ((uint32_t)(long)(int)(((uint32_t)((uint64_t)(*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x30))) >> 0x30) & 0x7ff) + 1
                             ) >> 1);
  if (param_4 == 0) {
    uVar1 = lVar2 << 0x24;
  }
  else {
    uVar1 = lVar2 * 0x10 + 8 << 0x20;
  }
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20))) =
       ((long)param_2 -
       ((uint32_t)(long)(int)(((uint32_t)((uint32_t)(*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x30))) >> 0x10) & 0x7ff) + 1) >>
       1)) * 0x10 | uVar1;
  return 0;
}


RT_GUEST_FN(0x0026f3c8, guest::uiEnvCenterPosB)

}  // namespace guest
