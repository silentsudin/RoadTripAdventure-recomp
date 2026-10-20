// guest:0x00264cb0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SubstituteNameTokens(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
uint32_t __src;
uint32_t pcVar1;
uint32_t acStack_50 = rtguest::allocFrame(48);
  
  pcVar1 = param_1;
  while( true ) {
    while( true ) {
      if ((*(int8_t*)rtguest::G((uint32_t)pcVar1)) == '\0') {
        return (uint32_t)0x0;
      }
      if ((*(int8_t*)rtguest::G((uint32_t)pcVar1)) == '$') break;
      pcVar1 = pcVar1 + 1;
    }
    if ((*(int8_t*)rtguest::G((uint32_t)pcVar1 + (uint32_t)(1) * sizeof(int8_t))) == 'N') break;
    pcVar1 = pcVar1 + 1;
  }
  (uint32_t)(uintptr_t)(::strcpy((char *)rtguest::G((uint32_t)(acStack_50)), (char *)rtguest::G((uint32_t)(pcVar1 + 2))));
  __src = (uint32_t)GetSlotDataPtr(0,0);
  (uint32_t)(uintptr_t)(::strcpy((char *)rtguest::G((uint32_t)(pcVar1)), (char *)rtguest::G((uint32_t)(__src))));
  pcVar1 = (uint32_t)(uintptr_t)(::strcat((char *)rtguest::G((uint32_t)(param_1)), (char *)rtguest::G((uint32_t)(acStack_50))));
  return pcVar1;
}


RT_GUEST_FN(0x00264cb0, guest::SubstituteNameTokens)

}  // namespace guest
