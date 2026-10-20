// guest:0x00258f70 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t PlayBgmSlot3(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
  MusicRpcPlayBgm(3,0);
  return 0;
}


RT_GUEST_FN(0x00258f70, guest::PlayBgmSlot3)

}  // namespace guest
