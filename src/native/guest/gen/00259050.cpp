// guest:0x00259050 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SoundFadeOutChannels(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  SoundRpcCmd26(0);
  SoundRpcCmd26(1);
  return 0;
}


RT_GUEST_FN(0x00259050, guest::SoundFadeOutChannels)

}  // namespace guest
