// guest:0x00234e98 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t InitMode17(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(param_1)), 0, 0x8cb0));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8004))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8008))) = 0x17;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x842c))) = param_4;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8420))) = param_2;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8424))) = param_3;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8428))) = param_3;
  return 0;
}


RT_GUEST_FN(0x00234e98, guest::InitMode17)

}  // namespace guest
