// guest:0x00235050 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ResetMode19(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
uint32_t pvVar1;
  
  pvVar1 = (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(param_1)), 0, 0x8cb0));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8004))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8008))) = 0x19;
  return pvVar1;
}


RT_GUEST_FN(0x00235050, guest::ResetMode19)

}  // namespace guest
