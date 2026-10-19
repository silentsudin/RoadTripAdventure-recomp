// guest:0x00258f50 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t PlayBgmSlot2(uint32_t q0, ...) {
  MusicRpcPlayBgm(2,0);
  return 0;
}


RT_GUEST_FN(0x00258f50, guest::PlayBgmSlot2)

}  // namespace guest
