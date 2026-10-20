// guest:0x00225588 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ResetCarEffectSlots(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
uint32_t pvVar1;
  
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(0x0172ff70)), 0, 0x800));
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(0x01730770)), 0, 0x80));
  pvVar1 = (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(0x017307f0)), 0, 0x400));
  return pvVar1;
}


RT_GUEST_FN(0x00225588, guest::ResetCarEffectSlots)

}  // namespace guest
