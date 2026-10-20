// guest:0x0022c630 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t PlayEventSoundStub(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
  PlayUiSound(0x8035);
  return 0;
}


RT_GUEST_FN(0x0022c630, guest::PlayEventSoundStub)

}  // namespace guest
