// guest:0x00273c40 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t scePadStateIntToStr(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
uint32_t pcVar1;
  
  if (param_1 < 8) {
    pcVar1 = (uint32_t)(uintptr_t)(::strcpy((char *)rtguest::G((uint32_t)(param_2)), (char *)rtguest::G((uint32_t)((*(uint32_t*)rtguest::G((uint32_t)(0x2e6ec8u + (uint32_t)((int)param_1) * 4)))))));
    return pcVar1;
  }
  (*(int8_t*)rtguest::G((uint32_t)param_2)) = '\0';
  return 0x330000u;
}


RT_GUEST_FN(0x00273c40, guest::scePadStateIntToStr)

}  // namespace guest
