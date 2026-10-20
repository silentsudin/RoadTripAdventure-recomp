// guest:0x00234c80 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetFlagConfig(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
uint32_t __s;
  
  param_1 = param_1 + param_2 * 0x408;
  __s = (uint32_t)(param_1 + 0x8774);
  if ((*(uint32_t*)rtguest::G((uint32_t)__s)) != (uint32_t)(param_3 != 0)) {
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x2c));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x878c))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)__s)) = (uint32_t)(param_3 != 0);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8778))) = 0x140;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x877c))) = 0x19c;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8784))) = 2;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8788))) = 0x0029fa68;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8780))) = 1;
  }
  return 0;
}


RT_GUEST_FN(0x00234c80, guest::SetFlagConfig)

}  // namespace guest
