// guest:0x00273b48 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t scePadRead(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(48);
uint32_t __src;
  undefined4 uVar1;
  
  if ((*(uint32_t*)rtguest::G((uint32_t)0x1773220 + (uint32_t)((int)param_1 * 0x1c + param_2 * 7) * 4)) == 0) {
    uVar1 = 0;
  }
  else {
    __src = (uint32_t)scePadGetDmaStr(param_1);
    (uint32_t)(uintptr_t)(::memcpy((char *)rtguest::G((uint32_t)(param_3)), (char *)rtguest::G((uint32_t)(__src)), (long)(*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)__src + 0x60)))));
    uVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)__src + 0x60)));
  }
  return uVar1;
}


RT_GUEST_FN(0x00273b48, guest::scePadRead)

}  // namespace guest
