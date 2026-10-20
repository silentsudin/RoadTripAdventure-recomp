// guest:0x0024fb38 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetGamePausedState(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
  if (param_2 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) | 0x2000;
    SoundStopAllChannels();
    return 0;
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) & 0xffffdfff;
  SoundFadeOutChannels();
  return 0;
}


RT_GUEST_FN(0x0024fb38, guest::SetGamePausedState)

}  // namespace guest
