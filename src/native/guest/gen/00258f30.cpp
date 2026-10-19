// guest:0x00258f30 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t PlayBgmSlot1(uint32_t q0, ...) {
  MusicRpcPlayBgm(1,0);
  return 0;
}


RT_GUEST_FN(0x00258f30, guest::PlayBgmSlot1)

}  // namespace guest
