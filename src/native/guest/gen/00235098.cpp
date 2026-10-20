// guest:0x00235098 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ResetMode1A(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(param_1)), 0, 0x8cb0));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8004))) = param_2;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8008))) = 0x1a;
  return 0;
}


RT_GUEST_FN(0x00235098, guest::ResetMode1A)

}  // namespace guest
