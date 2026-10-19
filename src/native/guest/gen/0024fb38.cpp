// guest:0x0024fb38 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetGamePausedState(uint32_t param_1, uint32_t param_2, ...) {
  if (param_2 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) | 0x2000;
    SoundStopAllChannels();
    return 0;
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) & 0xffffdfff;
  SoundFadeOutChannels();
}


RT_GUEST_FN(0x0024fb38, guest::SetGamePausedState)

}  // namespace guest
