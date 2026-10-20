// guest:0x00258fb0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t PlayBgmSlot5(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  MusicRpcPlayBgm(5,0);
  return 0;
}


RT_GUEST_FN(0x00258fb0, guest::PlayBgmSlot5)

}  // namespace guest
